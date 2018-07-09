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

Service::Service(const XML::Element& e) :
  element_{const_cast<XML::Element*>(&e)}
{}

Service::Service(const Service&) = default;
Service& Service::operator=(const Service&) = default;


std::string Service::cls(void) const {
  CHECK_NE(this->element_, nullptr);
  const XML::Attribute* attr = (*this->element_)["android:name"];
  if (attr == nullptr) {
    return "";
  }
  return attr->svalue();
}

bool Service::is_exported(void) const {
  CHECK_NE(this->element_, nullptr);
  const XML::Attribute* attr = (*this->element_)["android:exported"];
  if (attr == nullptr) {
    return true;
  }
  return static_cast<bool>(attr->ivalue());
}

size_t Service::priority(void) const {
  return 1;
}

bool Service::is_enabled(void) const {
  return true;
}

bool Service::is_isolated(void) const {
  return true;
}

Service::intent_actions_t Service::actions(void) const {
  CHECK_NE(this->element_, nullptr);
  Service::intent_actions_t actions;
  XML::const_elements_t intent_filters = this->element_->find_elements("intent-filter");

  for  (const XML::Element* e : intent_filters) {
    XML::const_elements_t actions_elements = e->find_elements("action");

    for (const XML::Element* a : actions_elements) {
      std::string action_str = a->get("android:name")->svalue();
      actions.push_back(action_str);
    }
  }

  return actions;

}

Service::intent_categories_t Service::categories(void) const {

  CHECK_NE(this->element_, nullptr);
  Service::intent_categories_t categories;
  XML::const_elements_t intent_filters = this->element_->find_elements("intent-filter");

  for  (const XML::Element* e : intent_filters) {
    XML::const_elements_t cat_elements = e->find_elements("category");

    for (const XML::Element* c : cat_elements) {
      std::string cat_str = c->get("android:name")->svalue();
      categories.push_back(cat_str);
    }
  }

  return categories;
}

std::ostream& operator<<(std::ostream& os, const Service& m) {
  static const char* SPACE = "   ";
  os << m.cls();
  if (m.is_exported()) {
    os << " - EXPORTED";
  }

  auto&& actions = m.actions();
  if (actions.size() > 0) {
    os << std::endl;
    for (const std::string& action : m.actions()) {
      os << SPACE << action << std::endl;
    }
  }
  return os;
}


}
