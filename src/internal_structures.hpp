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
#ifndef AMI_ANDROID_INTERNAL_STRUCTURES_H_
#define AMI_ANDROID_INTERNAL_STRUCTURES_H_

namespace AMI {
namespace details {
using res_string_pool_ref = uint32_t;
using res_table_ref       = uint32_t;

// These structures/enums comes from AOSP
// frameword_base/libs/androidfw/include/androidfw/ResourceTypes.h
struct res_value {
    uint16_t size;
    uint8_t res0;
    uint8_t data_type;
    uint32_t data;

    // Type of the data value.
    enum : uint8_t {
      TYPE_NULL              = 0x00,
      TYPE_REFERENCE         = 0x01,
      TYPE_ATTRIBUTE         = 0x02,
      TYPE_STRING            = 0x03,
      TYPE_FLOAT             = 0x04,
      TYPE_DIMENSION         = 0x05,
      TYPE_FRACTION          = 0x06,
      TYPE_DYNAMIC_REFERENCE = 0x07,
      TYPE_DYNAMIC_ATTRIBUTE = 0x08,
      TYPE_INT_DEC           = 0x10,
      TYPE_INT_HEX           = 0x11,
      TYPE_INT_BOOLEAN       = 0x12,
      TYPE_INT_COLOR_ARGB8   = 0x1c,
      TYPE_INT_COLOR_RGB8    = 0x1d,
      TYPE_INT_COLOR_ARGB4   = 0x1e,
      TYPE_INT_COLOR_RGB4    = 0x1f,
    };

    // Structure of complex data values (TYPE_UNIT and TYPE_FRACTION)
    enum {
      COMPLEX_UNIT_PX = 0,
      COMPLEX_UNIT_DIP = 1,
      COMPLEX_UNIT_SP = 2,
      COMPLEX_UNIT_PT = 3,
      COMPLEX_UNIT_IN = 4,
      COMPLEX_UNIT_MM = 5,

      COMPLEX_UNIT_FRACTION = 0,
      COMPLEX_UNIT_FRACTION_PARENT = 1,

      COMPLEX_RADIX_23p0 = 0,
      COMPLEX_RADIX_16p7 = 1,
      COMPLEX_RADIX_8p15 = 2,
      COMPLEX_RADIX_0p23 = 3,
    };

    // Possible data values for TYPE_NULL.
    enum {
      DATA_NULL_UNDEFINED = 0,
      DATA_NULL_EMPTY     = 1
    };
};


struct res_chunk_header {
  uint16_t type;
  uint16_t header_size;
  uint32_t size;
};

struct xml_tree_node {
  res_chunk_header    header;
  uint32_t            line_number;
  res_string_pool_ref comment;
};

struct xml_tree_cdata_ext {
  res_string_pool_ref data;
  res_value           typed_data;
};

struct xml_tree_namespace_ext {
  res_string_pool_ref prefix;
  res_string_pool_ref uri;
};

struct xml_tree_end_element_ext {
  res_string_pool_ref ns;
  res_string_pool_ref name;
};

struct xml_tree_attr_ext {
  res_string_pool_ref ns;
  res_string_pool_ref name;
  uint16_t attribute_start;
  uint16_t attribute_size;
  uint16_t attribute_count;
  uint16_t id_index;
  uint16_t class_index;
  uint16_t style_index;
};

struct xml_tree_attribute {
  res_string_pool_ref ns;
  res_string_pool_ref name;
  res_string_pool_ref raw_value;
  res_value           typed_value;
};

struct res_string_pool_header {
  res_chunk_header header;
  uint32_t string_count;
  uint32_t style_count;
  uint32_t flags;
  uint32_t strings_start;
  uint32_t styles_start;
};

struct res_string_pool_span {
  res_string_pool_ref name;
  uint32_t            first_char;
  uint32_t            last_char;
};

enum {
  RES_NULL_TYPE                = 0x0000,
  RES_STRING_POOL_TYPE         = 0x0001,
  RES_TABLE_TYPE               = 0x0002,
  RES_XML_TYPE                 = 0x0003,

  RES_XML_FIRST_CHUNK_TYPE     = 0x0100,
  RES_XML_START_NAMESPACE_TYPE = 0x0100,
  RES_XML_END_NAMESPACE_TYPE   = 0x0101,
  RES_XML_START_ELEMENT_TYPE   = 0x0102,
  RES_XML_END_ELEMENT_TYPE     = 0x0103,
  RES_XML_CDATA_TYPE           = 0x0104,
  RES_XML_LAST_CHUNK_TYPE      = 0x017f,
  RES_XML_RESOURCE_MAP_TYPE    = 0x0180,

  RES_TABLE_PACKAGE_TYPE       = 0x0200,
  RES_TABLE_TYPE_TYPE          = 0x0201,
  RES_TABLE_TYPE_SPEC_TYPE     = 0x0202,
  RES_TABLE_LIBRARY_TYPE       = 0x0203
};

}
}
#endif
