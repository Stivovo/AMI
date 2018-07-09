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
#include <fstream>
#include <algorithm>
#include "ManifestReader.hpp"
#include "macros.h"

namespace AMI {
namespace details {

ManifestReader::ManifestReader(const std::string& filename) {
  std::ifstream ifs{filename, std::ios::in | std::ios::binary};
  CHECK(static_cast<bool>(ifs));

  ifs.unsetf(std::ios::skipws);
  ifs.seekg(0, std::ios::end);
  //assert(ifs.tellg() > 0);
  this->size_ = static_cast<uint64_t>(ifs.tellg());
  ifs.seekg(0, std::ios::beg);

  // reserve capacity
  this->raw_.resize(this->size(), 0);

  std::move(
    std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(),
    std::begin(this->raw_)
  );
}


ManifestReader::operator bool() const {
  return this->pos_ < this->raw_.size();
}


ManifestReader::ManifestReader(const std::vector<uint8_t>& data) :
  raw_{data},
  pos_{0},
  size_{data.size()}
{}

ManifestReader::ManifestReader(std::vector<uint8_t>&& data) :
  raw_{std::move(data)},
  pos_{0},
  size_{raw_.size()}
{}


ManifestReader::ManifestReader(uint8_t* buffer, size_t size) :
  raw_{buffer, buffer + size},
  pos_{0},
  size_{size}
{}

uint64_t ManifestReader::size(void) const {
  return this->size_;
}

void ManifestReader::setpos(size_t pos) const {
  this->pos_ = pos;
}

void ManifestReader::increment_pos(size_t value) const {
  this->pos_ += value;
}


uint64_t ManifestReader::read_uleb128(void) const {
  uint64_t value = 0;
  unsigned shift = 0;
  uint8_t byte_read;
  do {
    byte_read = this->read<uint8_t>();
    value += static_cast<uint64_t>(byte_read & 0x7f) << shift;
    shift += 7;
  } while (byte_read >= 128);

  return value;
}


uint64_t ManifestReader::read_sleb128(void) const {
  int64_t  value = 0;
  unsigned shift = 0;
  uint8_t byte_read;
  do {
    byte_read = this->read<uint8_t>();
    value += static_cast<uint64_t>(byte_read & 0x7f) << shift;
    shift += 7;
  } while (byte_read >= 128);


  // Sign extend
  if ((value & 0x40) != 0) {
    value |= static_cast<int64_t>(-1) << shift;
  }

  return value;
}


const char* ManifestReader::read_string(void) const {
  const char* str = reinterpret_cast<const char*>(this->raw_.data() + this->pos_);
  while (this->pos_ < this->raw_.size() and this->raw_[this->pos_] != 0) {
    ++this->pos_;
  }
  return str;
}

const std::vector<uint8_t>& ManifestReader::content(void) const {
  return this->raw_;
}


float ManifestReader::transform(uint32_t x) {
  static constexpr size_t COMPLEX_MANTISSA_MASK  = 0xffffff;
  static constexpr size_t COMPLEX_MANTISSA_SHIFT = 8;

  static constexpr size_t COMPLEX_RADIX_MASK  = 0x3;
  static constexpr size_t COMPLEX_RADIX_SHIFT = 4;

  static constexpr float MANTISSA_MULT = 1.0f / (1 << COMPLEX_MANTISSA_SHIFT);

  static constexpr float RADIX_MULTS[] = {
    MANTISSA_MULT * 1.0f,
    MANTISSA_MULT * 1.0f / (1 << 7),
    MANTISSA_MULT * 1.0f / (1 << 15),
    MANTISSA_MULT * 1.0f / (1 << 23)
  };

  const size_t index = (x >> COMPLEX_RADIX_SHIFT) & COMPLEX_RADIX_MASK;
  float value = (x & (COMPLEX_MANTISSA_MASK << COMPLEX_MANTISSA_SHIFT)) * RADIX_MULTS[index];

  return value;
}

}
}
