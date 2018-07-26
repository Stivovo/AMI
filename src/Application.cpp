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

Application::Application(void) :
  element_{nullptr}
{}

Application::Application(const Application&) = default;
Application& Application::operator=(const Application&) = default;

Application::Application(const XML::Element& e) :
  element_{const_cast<XML::Element*>(&e)}
{}

Application::operator bool() const {
  return this->element_ != nullptr;
}


std::string Application::cls(void) const {
  if (not *this) {
    return "";
  }
  const XML::Attribute* name = (*this->element_)["android:name"];
  if (name) {
    return name->svalue();
  }
  return "";
}

bool Application::is_debuggable(void) const {
  if (not *this) {
    return false;
  }

  const XML::Attribute* attr = (*this->element_)["android:debuggable"];
  if (attr) {
    return attr->ivalue();
  }
  return false;
}

std::string Application::process(void) const {
  if (not *this) {
    return "";
  }

  const XML::Attribute* attr = (*this->element_)["android:process"];
  if (attr) {
    return attr->svalue();
  }
  return "";
}

std::string Application::backup_agent(void) const {
  if (not *this) {
    return "";
  }

  const XML::Attribute* attr = (*this->element_)["android:backupAgent"];
  if (attr) {
    return attr->svalue();
  }
  return "";
}

std::ostream& operator<<(std::ostream& os, const Application& m) {
  if (not m.cls().empty()) {
    os << "Class: " << m.cls() << std::endl;
  }

  if (not m.process().empty()) {
    os << "Process: " << m.process() << std::endl;
  }

  if (not m.backup_agent().empty()) {
    os << "Backup Agent: " << m.backup_agent() << std::endl;
  }

  os << "Is debuggable: " << std::boolalpha << m.is_debuggable() << std::endl;
  return os;
}

}
