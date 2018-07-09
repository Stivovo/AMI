/* Copyright 2018 R. Thomas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "AndroidManifestImpl.hpp"
#include "ManifestReader.hpp"
#include "macros.h"
#include "utf8.h"
#include "PrettyPrinter.hpp"
#include "res_val.hpp"
#include "internal_structures.hpp"

#include <iostream>
#include <fstream>
#include <zip.h>

namespace AMI {
namespace details {

bool is_apk(const std::string& path) {
  static const char MAGIC[] = {'P', 'K', 0x03, 0x04};
  char magic_read[sizeof(MAGIC)];
  std::fill(magic_read, magic_read + sizeof(magic_read), 0);

  if (std::ifstream ifs{path, std::ios::in | std::ios::binary}) {

    if (not ifs.read(magic_read, sizeof(magic_read))) {
      return false;
    }

    if (not std::equal(MAGIC, MAGIC + sizeof(MAGIC), magic_read)) {
      return false;
    }

    return true;
  }

  return false;
}

bool is_axml(const std::string& path) {
  static const char MAGIC[] = {0x03, 0x00};
  char magic_read[sizeof(MAGIC)];
  std::fill(magic_read, magic_read + sizeof(magic_read), 0);

  if (std::ifstream ifs{path, std::ios::in | std::ios::binary}) {

    if (not ifs.read(magic_read, sizeof(magic_read))) {
      return false;
    }

    if (not std::equal(MAGIC, MAGIC + sizeof(MAGIC), magic_read)) {
      return false;
    }

    return true;
  }

  return false;
}

const XML::Manifest& AndroidManifestImpl::manifest(void) const {
  CHECK(static_cast<bool>(this->manifest_));
  return *this->manifest_;
}

XML::Manifest& AndroidManifestImpl::manifest(void) {
  CHECK(static_cast<bool>(this->manifest_));
  return *this->manifest_;
}

const char* AndroidManifestImpl::get_resource_string(size_t id) {

  auto&& it = res_map.find(id);
  if (it == std::end(res_map)) {
    return nullptr;
  }
  return it->second;
}

AndroidManifestImpl* AndroidManifestImpl::open(const std::string& path) {
  if (is_apk(path)) {
    return AndroidManifestImpl::from_apk(path);
  }

  if (is_axml(path)) {
    return AndroidManifestImpl::from_axml(path);
  }

  return nullptr;
}

AndroidManifestImpl* AndroidManifestImpl::from_apk(const std::string& path) {
  static const std::string ANDROID_MANIFEST_FILE = "AndroidManifest.xml";

  int err = 0;

  zip *z = zip_open(path.c_str(), ZIP_RDONLY, &err);

  if (z == nullptr) {
    zip_close(z);
    return nullptr;
  }

  // Look for AndroidManifest.xml
  zip_file_t* z_android_manifest = zip_fopen(z, ANDROID_MANIFEST_FILE.c_str(), ZIP_FL_UNCHANGED);

  if (z_android_manifest == nullptr) {
    zip_close(z);
    return nullptr;
  }
  struct zip_stat st;
  zip_stat_init(&st);
  zip_stat(z, ANDROID_MANIFEST_FILE.c_str(), ZIP_STAT_SIZE, &st);
  std::vector<uint8_t> raw(st.size, 0);

  zip_int64_t size_read = zip_fread(z_android_manifest, raw.data(), raw.size());

  if (size_read <= 0) {
    zip_fclose(z_android_manifest);
    zip_close(z);
    return nullptr;
  }

  zip_close(z);
  zip_fclose(z_android_manifest);
  return new AndroidManifestImpl{raw.data(), raw.size(), path};
}

AndroidManifestImpl* AndroidManifestImpl::from_axml(const std::string& path) {
  std::ifstream ifs{path, std::ios::in | std::ios::binary};
  CHECK(static_cast<bool>(ifs));

  ifs.unsetf(std::ios::skipws);
  ifs.seekg(0, std::ios::end);

  size_t size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  std::vector<uint8_t> raw;
  raw.resize(size, 0);

  std::move(
    std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(),
    std::begin(raw)
  );
  return new AndroidManifestImpl{raw.data(), raw.size(), path};

}

AndroidManifestImpl::AndroidManifestImpl(uint8_t* buffer, size_t size, const std::string& path) :
  path_{path}
{

  ManifestReader sr{buffer, size};

  const res_chunk_header& xml_header = sr.read<res_chunk_header>();
  CHECK_EQ(xml_header.type, RES_XML_TYPE);

  this->manifest_ = std::unique_ptr<XML::Manifest>{new XML::Manifest{}};

  sr.xml_stack.push_back(this->manifest_.get());

  while (sr.pos() < xml_header.size) {
    size_t saved_pos = sr.pos();
    const res_chunk_header& header = sr.peek<res_chunk_header>();
    switch (header.type) {
      case RES_STRING_POOL_TYPE:
        {
          this->parse<RES_STRING_POOL_TYPE>(sr);
          break;
        }

      case RES_XML_RESOURCE_MAP_TYPE:
        {
          this->parse<RES_XML_RESOURCE_MAP_TYPE>(sr);
          break;
        }

      case RES_XML_START_NAMESPACE_TYPE:
        {
          this->parse<RES_XML_START_NAMESPACE_TYPE>(sr);
          break;
        }

      case RES_XML_END_ELEMENT_TYPE:
        {
          this->parse<RES_XML_END_ELEMENT_TYPE>(sr);
          break;
        }

      case RES_XML_END_NAMESPACE_TYPE:
        {
          this->parse<RES_XML_END_NAMESPACE_TYPE>(sr);
          break;
        }

      case RES_XML_START_ELEMENT_TYPE:
        {
          this->parse<RES_XML_START_ELEMENT_TYPE>(sr);
          break;
        }

      case RES_XML_CDATA_TYPE:
        {
          this->parse<RES_XML_CDATA_TYPE>(sr);
          break;
        }

      default:
        {

        }
    }

    sr.setpos(saved_pos + header.size);

  }
}

template<>
void AndroidManifestImpl::parse<RES_STRING_POOL_TYPE>(const ManifestReader& sr) {
  static constexpr uint32_t UTF8_MASK   = 1 << 8;
  /* static constexpr uint32_t SORTED_MASK = 1 << 0; */

  const res_string_pool_header& pool = sr.read<res_string_pool_header>();

  bool is_utf8   = pool.flags & UTF8_MASK;
  /* bool is_sorted = pool.flags & SORTED_MASK; */

  auto decode_utf16_length = [&] () {
    size_t len = sr.read<uint16_t>();
    if ((len & 0x800) != 0) {
      len |= ((len & 0x7FFF) << 16) | sr.read<uint16_t>();
    }
    return len;
  };

  auto decode_utf8_length = [&] () {
    size_t len = sr.read<uint8_t>();
    if ((len & 0x80) != 0) {
      sr.read<uint8_t>();
    }
    return len;
  };

  std::vector<uint32_t> str_offsets;
  str_offsets.reserve(pool.string_count);
  this->string_pool_.reserve(pool.string_count);

  // Fill strings offsets
  for (size_t i = 0; i < pool.string_count; ++i) {
    str_offsets.push_back(sr.read<uint32_t>());
  }

  for (uint32_t soff : str_offsets) {
    sr.setpos(pool.strings_start + 8 + soff);
    if (is_utf8) {
      size_t len = decode_utf8_length();
      if (len > 0) {
        const char* c_str = sr.read_string();
        this->string_pool_.emplace_back(c_str);
      } else {
        this->string_pool_.emplace_back("");
      }

    } else {
      size_t len = decode_utf16_length();
      const uint16_t* utf16_ptr = &sr.peek<uint16_t>();

      std::u16string str{utf16_ptr, utf16_ptr + len};
      std::string u8;
      utf8::utf16to8(std::begin(str), std::end(str), std::back_inserter(u8));
      this->string_pool_.push_back(std::move(u8));

    }
  }
}

template<>
void AndroidManifestImpl::parse<RES_XML_RESOURCE_MAP_TYPE>(const ManifestReader& sr) {
  const res_chunk_header& header = sr.read<res_chunk_header>();
  const size_t nb_elements = (header.size - sizeof(res_chunk_header)) / sizeof(uint32_t);
  sr.res_ids.reserve(nb_elements);

  for (size_t i = 0; i < nb_elements; ++i) {
    sr.res_ids.push_back(sr.read<uint32_t>());
  }
}

template<>
void AndroidManifestImpl::parse<RES_XML_END_NAMESPACE_TYPE>(const ManifestReader& sr) {
  /* const xml_tree_node& node        = */ sr.read<xml_tree_node>();
  /* const xml_tree_namespace_ext& ns = */ sr.read<xml_tree_namespace_ext>();

  sr.xml_stack.pop_back();
}

template<>
void AndroidManifestImpl::parse<RES_XML_START_ELEMENT_TYPE>(const ManifestReader& sr) {
  //
  // Most of the implementation can be found
  // in framework_base/tools/aapt/XMLNode.cpp:1462
  // status_t XMLNode::flatten_node(const StringPool& strings, const sp<AaptFile>& dest,


  static constexpr uint32_t NONE = -1u;
  /* const xml_tree_node& node     =*/ sr.read<xml_tree_node>();
  const xml_tree_attr_ext& element = sr.read<xml_tree_attr_ext>();

  std::string element_name = this->string_at(element.name);

  auto ns_prefix = [&] (const std::string& uri) -> std::string {
    XML::Node* current = sr.xml_stack.back();
    do {
      if (current->is<XML::Namespace>()) {
        const XML::Namespace* current_ns = current->as<XML::Namespace>();
        if (current_ns->uri() == uri) {
          return current_ns->prefix();
        }
      }
      current = current->parent();
    } while (current->parent());
    return "";
  };

  XML::Element& xml_element = dynamic_cast<XML::Element&>(sr.xml_stack.back()->add(new XML::Element{element_name}));

  for (size_t i = 0; i < element.attribute_count; ++i) {
    const xml_tree_attribute& attribute = sr.read<xml_tree_attribute>();


    std::string ns = "";
    if (attribute.ns != NONE) {
      uint32_t uri = attribute.ns;
      ns = ns_prefix(this->string_at(uri));
      if (not ns.empty()) {
        ns += ":";
      }
    }

    std::string attribute_name = this->string_at(attribute.name);
    if (attribute_name.empty()) {
      //TODO: Here we assume android prefix
      attribute_name += get_resource_string(sr.res_id_at(attribute.name));
      ns = "android:";
    }

    // raw_value is not nonte, thus it's a string
    if (attribute.raw_value != NONE) {
      std::string value = this->string_at(attribute.raw_value);
      xml_element.add_attribute(ns + attribute_name, value, XML::Attribute::TYPES::STRING);
    }
    // Otherwise it a res_value
    else {
      res_value typed_val = attribute.typed_value;
      switch (typed_val.data_type) {
        case res_value::TYPE_REFERENCE:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::REFERENCE);
            break;
          }

        case res_value::TYPE_ATTRIBUTE:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::ATTRIBUTE);
            break;
          }

        case res_value::TYPE_STRING:
          {
            //std::string value = this->string_at(typed_val.data);
            //xml_element.add_attribute(ns + attribute_name, value, XML::Attribute::TYPES::STRING);
            break;
          }

        case res_value::TYPE_FLOAT:
          {
            xml_element.add_attribute(ns + attribute_name, ManifestReader::transform(typed_val.data), XML::Attribute::TYPES::FLOAT);
            break;
          }

        case res_value::TYPE_DIMENSION:
          {
            xml_element.add_attribute(ns + attribute_name, ManifestReader::transform(typed_val.data), XML::Attribute::TYPES::DIMENSION);
            break;
          }

        case res_value::TYPE_FRACTION:
          {
            break;
          }

        case res_value::TYPE_DYNAMIC_REFERENCE:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::DYNAMIC_REFERENCE);
            break;
          }

        case res_value::TYPE_DYNAMIC_ATTRIBUTE:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::DYNAMIC_ATTRIBUTE);
            break;
          }

        case res_value::TYPE_INT_DEC:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_DEC);
            break;
          }

        case res_value::TYPE_INT_HEX:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_HEX);
            break;
          }

        case res_value::TYPE_INT_BOOLEAN:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_BOOLEAN);
            break;
          }

        case res_value::TYPE_INT_COLOR_ARGB8:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_COLOR_ARGB8);
            break;
          }

        case res_value::TYPE_INT_COLOR_RGB8:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_COLOR_RGB8);
            break;
          }

        case res_value::TYPE_INT_COLOR_ARGB4:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_COLOR_ARGB4);
            break;
          }

        case res_value::TYPE_INT_COLOR_RGB4:
          {
            xml_element.add_attribute(ns + attribute_name, typed_val.data, XML::Attribute::TYPES::INT_COLOR_RGB4);
            break;
          }

      }


    }
  }
  sr.xml_stack.push_back(&xml_element);

}

template<>
void AndroidManifestImpl::parse<RES_XML_CDATA_TYPE>(const ManifestReader& sr) {
  /* const xml_tree_node& node    = */ sr.read<xml_tree_node>();
  const xml_tree_cdata_ext& cdata =    sr.read<xml_tree_cdata_ext>();
  const std::string& txt          =    this->string_at(cdata.data);

  std::unique_ptr<XML::CData> data{new XML::CData{std::move(txt)}};
  sr.xml_stack.back()->add(data.release());
}

template<>
void AndroidManifestImpl::parse<RES_XML_END_ELEMENT_TYPE>(const ManifestReader& sr) {
  /* const xml_tree_node& node          = */ sr.read<xml_tree_node>();
  /* const xml_tree_end_element_ext& ns = */ sr.read<xml_tree_end_element_ext>();
  sr.xml_stack.pop_back();
}

template<>
void AndroidManifestImpl::parse<RES_XML_START_NAMESPACE_TYPE>(const ManifestReader& sr) {
  /* const xml_tree_node& node        = */ sr.read<xml_tree_node>();
  const xml_tree_namespace_ext& ns =       sr.read<xml_tree_namespace_ext>();

  const std::string& prefix = this->string_at(ns.prefix);
  const std::string& uri    = this->string_at(ns.uri);

  XML::Node& added = sr.xml_stack.back()->add(new XML::Namespace{prefix, uri});
  sr.xml_stack.push_back(&added);


}


std::string AndroidManifestImpl::string_at(size_t index) const {
  CHECK_LT(index, this->string_pool_.size());
  const std::string& orginal = this->string_pool_[index];
  std::string clean = "";

  for (const uint8_t c : orginal) {
    if (std::isprint(c)) {
      clean += c;
    }
  }

  return clean;
}


AndroidManifestImpl::AndroidManifestImpl(const AndroidManifestImpl& other) :
  string_pool_{other.string_pool_},
  path_{other.path_},
  manifest_{other.manifest_->clone()}
{}

}
}
