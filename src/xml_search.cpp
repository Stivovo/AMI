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
#include "xml_search.hpp"
#include <iostream>
#include <algorithm>
namespace AMI {
namespace details {

XMLSearcher::XMLSearcher(const XMLSearcher::predicate_t& p, bool unique) :
  result_{},
  predicate_{p},
  unique_{unique}
{}


XML::const_elements_t XMLSearcher::elements_by_name(const XML::Node& root, const std::string& name, bool only_one) {
  auto p = [name] (const XML::Node& node) {
    return node.is<XML::Element>() and node.as<XML::Element>()->name() == name;
  };

  XMLSearcher visitor{p, only_one};
  root.accept(visitor);
  result_t r = std::move(visitor.result());
  XML::const_elements_t elements;
  elements.reserve(r.size());

  std::transform(
      std::make_move_iterator(std::begin(r)),
      std::make_move_iterator(std::end(r)),
      std::back_inserter(elements),
      [] (const XML::Node* n) {
        return n->as<XML::Element>();
      });
  return elements;
}


XML::const_attributes_t XMLSearcher::attributes_by_name(const XML::Node& root, const std::string& name, bool only_one) {
  auto p = [name] (const XML::Node& node) {
    return node.is<XML::Attribute>() and node.as<XML::Attribute>()->name() == name;
  };

  XMLSearcher visitor{p, only_one};
  root.accept(visitor);

  result_t r = std::move(visitor.result());
  XML::const_attributes_t attr;
  attr.reserve(r.size());

  std::transform(
      std::make_move_iterator(std::begin(r)),
      std::make_move_iterator(std::end(r)),
      std::back_inserter(attr),
      [] (const XML::Node* n) {
        return n->as<XML::Attribute>();
      });
  return attr;
}



bool XMLSearcher::try_insert(const XML::Node& node) {
  if (this->predicate_(node)) {
    this->result_.push_back(&node);
    return true;;
  }
  return false;
}

void XMLSearcher::visit(const XML::Manifest& node) {
  if (this->try_insert(node) and this->unique_) {
    return;
  }

  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }
}

void XMLSearcher::visit(const XML::Namespace& node) {

  if (this->try_insert(node) and this->unique_) {
    return;
  }
  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }

}

void XMLSearcher::visit(const XML::Element& node) {

  if (this->try_insert(node) and this->unique_) {
    return;
  }

  for (const XML::Node* node : node.attributes()) {
    this->visit(*node);
  }

  if (this->result_.size() > 0 and this->unique_) {
    return;
  }


  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }

}

void XMLSearcher::visit(const XML::Attribute& node) {
  if (this->try_insert(node) and this->unique_) {
    return;
  }

  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }
}

void XMLSearcher::visit(const XML::CData& node) {
  if (this->try_insert(node) and this->unique_) {
    return;
  }

  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }
}

XMLSearcher::result_t&& XMLSearcher::result(void) {
  return std::move(this->result_);
}

XMLSearcher::~XMLSearcher(void) = default;


}
}
