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
#ifndef AMI_ANDROID_MANIFEST_IMPL_H_
#define AMI_ANDROID_MANIFEST_IMPL_H_
#include <string>
#include <vector>

#include "AMI/visibility.h"
#include "AMI/XML.hpp"

#include "internal_structures.hpp"

namespace AMI {
namespace details {
class ManifestReader;

AMI_LOCAL bool is_apk(const std::string& path);
AMI_LOCAL bool is_axml(const std::string& path);

class AMI_LOCAL AndroidManifestImpl {
  public:
  static const char* get_resource_string(size_t id);

  static AndroidManifestImpl* open(const std::string& path);
  static AndroidManifestImpl* from_apk(const std::string& path);
  static AndroidManifestImpl* from_axml(const std::string& path);

  public:
  //AndroidManifestImpl(const std::string& path);
  AndroidManifestImpl(const AndroidManifestImpl& other);

  std::string string_at(size_t index) const;

  const XML::Manifest& manifest(void) const;
  XML::Manifest& manifest(void);

  private:
  using string_pool_t = std::vector<std::string>;
  AndroidManifestImpl(uint8_t* buffer, size_t size, const std::string& path = "");

  template<size_t T>
  void parse(const ManifestReader& sr);

  string_pool_t string_pool_;
  std::string path_;
  std::unique_ptr<XML::Manifest> manifest_;

};

template<>
void AndroidManifestImpl::parse<RES_STRING_POOL_TYPE>(const ManifestReader& sr);

template<>
void AndroidManifestImpl::parse<RES_XML_RESOURCE_MAP_TYPE>(const ManifestReader& sr);

template<>
void AndroidManifestImpl::parse<RES_XML_START_NAMESPACE_TYPE>(const ManifestReader& sr);

template<>
void AndroidManifestImpl::parse<RES_XML_END_NAMESPACE_TYPE>(const ManifestReader& sr);

template<>
void AndroidManifestImpl::parse<RES_XML_START_ELEMENT_TYPE>(const ManifestReader& sr);

template<>
void AndroidManifestImpl::parse<RES_XML_END_ELEMENT_TYPE>(const ManifestReader& sr);

template<>
void AndroidManifestImpl::parse<RES_XML_CDATA_TYPE>(const ManifestReader& sr);

}
}

#endif
