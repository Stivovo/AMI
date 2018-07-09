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
#include "PrettyPrinter.hpp"
#include <iostream>
namespace AMI {
namespace details {

PrettyPrinter::PrettyPrinter(std::ostream& os) :
  indent_{0},
  os_{os}
{}

void PrettyPrinter::visit(const XML::Manifest& node) {
  this->os() << "<manifest>" << std::endl;
  this->indent_ += IDENT_SIZE;
  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }
  this->indent_ -= IDENT_SIZE;
  this->os() << "</manifest>" << std::endl;;
}
void PrettyPrinter::visit(const XML::Namespace& node) {
  this->os() << "<namespace prefix:" << node.prefix() << " uri: " <<  node.uri() << std::endl;
  this->indent_ += IDENT_SIZE;
  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }
  this->indent_ -= IDENT_SIZE;
  this->os() << "</namespace>" << std::endl;
}

void PrettyPrinter::visit(const XML::Element& node) {
  this->os() << "<" << node.name();

  for (const XML::Attribute* attr : node.attributes()) {
    this->visit(*attr);
  }

  this->os(false) << ">" << std::endl;

  this->indent_ += IDENT_SIZE;
  for (const XML::Node* node : node.children()) {
    this->visit(*node);
  }
  this->indent_ -= IDENT_SIZE;
  this->os() << "</" << node.name() << ">" << std::endl << std::endl;

}
void PrettyPrinter::visit(const XML::Attribute& node) {
  this->os(false) << " " << node.name() << "=";

  switch (node.type()) {
    case XML::Attribute::TYPES::INT_DEC:
      {
        this->os(false) << std::dec << node.ivalue();
        break;
      }

    case XML::Attribute::TYPES::INT_HEX:
      {
        this->os(false) << std::hex << std::showbase << node.ivalue();
        break;
      }

    case XML::Attribute::TYPES::INT_BOOLEAN:
      {
        this->os(false) << std::boolalpha << static_cast<bool>(node.ivalue());
        break;
      }

    case XML::Attribute::TYPES::STRING:
      {
        this->os(false) << node.svalue();
        break;
      }

    case XML::Attribute::TYPES::REFERENCE:
      {
        this->os(false) << "@" << std::hex << std::showbase << node.ivalue();
        break;
      }

    case XML::Attribute::TYPES::ATTRIBUTE:
      {
        this->os(false) << "?" << std::hex << std::showbase << node.ivalue();
        break;
      }

    case XML::Attribute::TYPES::FLOAT:
      {
        this->os(false) << node.fvalue();
        break;
      }

    default:
      {
      }
  }
}

void PrettyPrinter::visit(const XML::CData& node) {
  this->os() << "<![CDATA[" << std::endl;
  this->indent_ += IDENT_SIZE;
  this->os() << node.cdata();
  this->indent_ -= IDENT_SIZE;
  this->os() << "]]>" << std::endl;
}


std::ostream& PrettyPrinter::os(bool indent) {
  if (indent) {
    this->os_ << std::string(this->indent_, ' ');
  }
  return this->os_;
}

PrettyPrinter::~PrettyPrinter(void) = default;


}
}
