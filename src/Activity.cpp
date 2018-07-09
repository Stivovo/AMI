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

#include "AMI/AMI.hpp"
#include "AndroidManifestImpl.hpp"
#include "macros.h"

namespace AMI {

Activity::Activity(const XML::Element& e) :
  element_{const_cast<XML::Element*>(&e)}
{}

Activity::Activity(const Activity&) = default;
Activity& Activity::operator=(const Activity&) = default;

std::string Activity::cls(void) const {
  CHECK_NE(this->element_, nullptr);
  const XML::Attribute* attr = (*this->element_)["android:name"];
  if (attr == nullptr) {
    return "";
  }
  return (*this->element_)["android:name"]->svalue();
}

std::ostream& operator<<(std::ostream& os, const Activity& m) {
  os << m.cls();
  return os;
}

}
