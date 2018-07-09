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
#ifndef AMI_DETAILS_MANIFEST_READER_H
#define AMI_DETAILS_MANIFEST_READER_H

#include <vector>
#include <string>
#include "macros.h"
namespace AMI {
namespace XML {
class Node;
}

namespace details {
class AndroidManifestImpl;

class ManifestReader {
  friend class AndroidManifestImpl;
  public:
  ManifestReader(const std::string& filename);
  ManifestReader(const std::vector<uint8_t>& data);
  ManifestReader(std::vector<uint8_t>&& data);
  ManifestReader(uint8_t* buffer, size_t size);

  uint64_t size(void) const;

  void setpos(size_t pos) const;
  void increment_pos(size_t value) const;

  inline size_t pos(void) const { return this->pos_; }

  operator bool() const;

  template<class T>
  const T& read(void) const;

  template<class T>
  const T& peek(void) const;

  uint64_t read_uleb128(void) const;
  uint64_t read_sleb128(void) const;
  const char* read_string(void) const;

  const std::vector<uint8_t>& content(void) const;

  private:
  static float transform(uint32_t x);
  inline uint32_t res_id_at(size_t idx) const {
    CHECK_LT(idx, this->res_ids.size());
    return this->res_ids[idx];
  }

  mutable std::vector<XML::Node*> xml_stack;
  std::vector<uint8_t> raw_;
  mutable size_t pos_{0};
  uint64_t size_;

  mutable std::vector<uint32_t> res_ids;

};


template<class T>
const T& ManifestReader::read(void) const {
  const T& tmp = this->peek<T>();
  this->increment_pos(sizeof(T));
  return tmp;
}

template<class T>
const T& ManifestReader::peek(void) const {
  return *reinterpret_cast<const T*>(this->raw_.data() + this->pos_);
}


}
}


#endif
