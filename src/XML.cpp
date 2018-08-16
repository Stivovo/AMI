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

#include "AMI/XML.hpp"
#include "PrettyPrinter.hpp"

#include <algorithm>
#include <iostream>
namespace AMI {
namespace XML {
// Node
// =====
Node::~Node(void) = default;

Node::Node(void) = default;

Node::Node(const Node& other) {
  this->children_.reserve(other.children_.size());
  for (const std::unique_ptr<Node>& c : other.children_) {
    this->children_.emplace_back(c->clone());
  }
}
const Node* Node::parent(void) const {
  return this->parent_;
}

Node* Node::parent(void) {
  return this->parent_;
}

Node& Node::add(Node* node) {
  this->children_.emplace_back(node);
  node->parent_ = this;
  return *this->children_.back();
}


Node::children_const_t Node::children(void) const {
  Node::children_const_t children;
  children.reserve(this->children_.size());
  std::transform(
      std::begin(this->children_),
      std::end(this->children_),
      std::back_inserter(children),
      [] (const std::unique_ptr<Node>& n) {
        return n.get();
      });
  return children;
}

Node::children_t Node::children(void) {

  Node::children_t children;
  children.reserve(this->children_.size());
  std::transform(
      std::begin(this->children_),
      std::end(this->children_),
      std::back_inserter(children),
      [] (std::unique_ptr<Node>& n) {
        return n.get();
      });
  return children;
}


const_elements_t Node::find_elements(const std::string& name, bool only_one) const {
  return details::XMLSearcher::elements_by_name(*this, name, only_one);
}


const_attributes_t Node::find_attributes(const std::string& name, bool only_one) const {
  return details::XMLSearcher::attributes_by_name(*this, name, only_one);
}


std::ostream& Node::dump(std::ostream& os) const {
  details::PrettyPrinter pp{os};
  this->accept(pp);
  return os;
}


// Manifest
// ========
Manifest::~Manifest(void) = default;

Manifest::Manifest(void) = default;
Manifest::Manifest(const Manifest&) = default;

Manifest* Manifest::clone(void) const {
  return new Manifest{*this};
}

void Manifest::accept(Visitor& visitor) const {
  visitor.visit(*this);
}


// Namespace
// =========
Namespace::~Namespace(void) = default;
Namespace::Namespace(const Namespace&) = default;

Namespace::Namespace(const std::string& prefix, const std::string& uri) :
  Node{},
  prefix_{prefix},
  uri_{uri}
{}


Namespace* Namespace::clone(void) const {
  return new Namespace{*this};
}

const std::string& Namespace::prefix(void) const {
  return this->prefix_;
}

const std::string& Namespace::uri(void) const {
  return this->uri_;
}

void Namespace::accept(Visitor& visitor) const {
  visitor.visit(*this);
}


// Element
// =======
Element::~Element(void) = default;

Element* Element::clone(void) const {
  return new Element{*this};
}

Element::Element(const std::string& name,  size_t nb_attributes) :
  name_{name}
{
  this->attributes_.reserve(nb_attributes);
}


Element::Element(const Element& other) :
  Node{other},
  name_{other.name_}
{
  this->attributes_.reserve(other.attributes_.size());
  for (auto&& c : other.attributes_) {
    this->attributes_[c.first] = std::unique_ptr<Attribute>{c.second->clone()};
  }
}

const std::string& Element::name(void) const {
  return this->name_;
}

Attribute& Element::add_attribute(const std::string& name, const std::string& value, Attribute::TYPES type) {
  this->attributes_[name] = std::unique_ptr<Attribute>{new Attribute{name, value, type}};
  return *this->attributes_[name].get();
}

Attribute& Element::add_attribute(const std::string& name, uint32_t value, Attribute::TYPES type) {
  this->attributes_[name] = std::unique_ptr<Attribute>{new Attribute{name, value, type}};
  return *this->attributes_[name].get();
}

Attribute& Element::add_attribute(const std::string& name, float value, Attribute::TYPES type) {
  this->attributes_[name] = std::unique_ptr<Attribute>{new Attribute{name, value, type}};
  return *this->attributes_[name].get();
}

const_attributes_t Element::attributes(void) const {
  const_attributes_t attr;
  attr.reserve(this->attributes_.size());

  std::transform(
      std::begin(this->attributes_),
      std::end(this->attributes_),
      std::back_inserter(attr),
      [] (const attributes_container_t::value_type& p) {
        return p.second.get();
      });
  return attr;
}

attributes_t Element::attributes(void) {
  attributes_t attr;
  attr.reserve(this->attributes_.size());

  std::transform(
      std::begin(this->attributes_),
      std::end(this->attributes_),
      std::back_inserter(attr),
      [] (const attributes_container_t::value_type& p) {
        return p.second.get();
      });
  return attr;
}

const Attribute* Element::operator[](const std::string& attr_name) const {
  const_attributes_t attr = this->find_attributes(attr_name, /* only_one */ true);
  if (attr.size() > 0) {
    return attr[0]->as<Attribute>();
  }
  return nullptr;

}

Attribute* Element::operator[](const std::string& attr_name) {
  return const_cast<Attribute*>(static_cast<const Element*>(this)->operator[](attr_name));
}


const Attribute* Element::get(const std::string& attr_name) const {
  return (*this)[attr_name];
}

Attribute* Element::get(const std::string& attr_name) {
  return (*this)[attr_name];
}

void Element::accept(Visitor& visitor) const {
  visitor.visit(*this);
}


// Attribute
// =========
Attribute::~Attribute(void) {
  switch (this->type_) {
    case TYPES::REFERENCE:
    case TYPES::ATTRIBUTE:
    case TYPES::DIMENSION:
    case TYPES::DYNAMIC_REFERENCE:
    case TYPES::DYNAMIC_ATTRIBUTE:
    case TYPES::INT_DEC:
    case TYPES::INT_HEX:
    case TYPES::INT_BOOLEAN:
    case TYPES::INT_COLOR_ARGB8:
    case TYPES::INT_COLOR_RGB8:
    case TYPES::INT_COLOR_ARGB4:
    case TYPES::INT_COLOR_RGB4:
      {
        delete this->integer_;
        break;
      }

    case TYPES::STRING:
      {
        delete this->string_;
        break;
      }

    case TYPES::FLOAT:
    case TYPES::FRACTION:
      {
        delete this->float_;
        break;
      }
    default:
      {
      }
  }
}

Attribute::Attribute(const Attribute& other) :
  Node::Node{other},
  name_{other.name_},
  type_{other.type_},
  string_{nullptr}
{

  switch (this->type_) {
    case TYPES::REFERENCE:
    case TYPES::ATTRIBUTE:
    case TYPES::DIMENSION:
    case TYPES::DYNAMIC_REFERENCE:
    case TYPES::DYNAMIC_ATTRIBUTE:
    case TYPES::INT_DEC:
    case TYPES::INT_HEX:
    case TYPES::INT_BOOLEAN:
    case TYPES::INT_COLOR_ARGB8:
    case TYPES::INT_COLOR_RGB8:
    case TYPES::INT_COLOR_ARGB4:
    case TYPES::INT_COLOR_RGB4:
      {
        this->integer_ = new uint32_t{*other.integer_};
        break;
      }

    case TYPES::STRING:
      {
        this->string_ = new std::string{*other.string_};
        break;
      }

    case TYPES::FLOAT:
    case TYPES::FRACTION:
      {
        this->float_ = new float{*other.float_};
        break;
      }
    default: {}

  }
}

Attribute::Attribute(const std::string& name, const std::string& val, TYPES type) :
  name_{name},
  type_{type},
  string_{new std::string{val}}
{}

Attribute::Attribute(const std::string& name, uint32_t val, TYPES type) :
  name_{name},
  type_{type},
  integer_{new uint32_t{val}}
{}

Attribute::Attribute(const std::string& name, float val, TYPES type) :
  name_{name},
  type_{type},
  float_{new float{val}}
{}

Attribute* Attribute::clone(void) const {
  return new Attribute{*this};
}

const std::string& Attribute::name(void) const {
  return this->name_;
}

Attribute::TYPES Attribute::type(void) const {
  return this->type_;
}

const std::string& Attribute::svalue(void) const {
  return *this->string_;
}

uint32_t Attribute::ivalue(void) const {
  return *this->integer_;
}

float Attribute::fvalue(void) const {
  return *this->float_;
}

void Attribute::accept(Visitor& visitor) const {
  visitor.visit(*this);
}


// CData
// =====
CData::~CData(void) = default;
CData::CData(const CData&) = default;

CData::CData(const std::string& cdata) :
  cdata_{cdata}
{}


const std::string& CData::cdata(void) const {
  return this->cdata_;
}

CData* CData::clone(void) const {
  return new CData{*this};
}

void CData::accept(Visitor& visitor) const {
  visitor.visit(*this);
}


// Visitor
// =======

Visitor::Visitor(void) = default;

void Visitor::visit(const Node& node) {
  node.accept(*this);
}
void Visitor::visit(const Manifest&) {}
void Visitor::visit(const Namespace&) {}
void Visitor::visit(const Element&) {}
void Visitor::visit(const Attribute&) {}
void Visitor::visit(const CData&) {}

Visitor::~Visitor(void) = default;


}
}
