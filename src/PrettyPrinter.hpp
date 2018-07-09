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
#ifndef AMI_DETAILS_PRETTY_PRINTER_H_
#define AMI_DETAILS_PRETTY_PRINTER_H_

#include <AMI/XML.hpp>

namespace AMI {
namespace details {

class PrettyPrinter : public XML::Visitor {
  public:
  static const size_t IDENT_SIZE = 2;
  using Visitor::visit;
  PrettyPrinter(void) = delete;
  PrettyPrinter(std::ostream& os);

  virtual void visit(const XML::Manifest& node) override;
  virtual void visit(const XML::Namespace& node) override;
  virtual void visit(const XML::Element& node) override;
  virtual void visit(const XML::Attribute& node) override;
  virtual void visit(const XML::CData& node) override;

  std::ostream& os(bool indent = true);

  virtual ~PrettyPrinter(void);

  private:
  size_t indent_;
  std::ostream& os_;

};


}
}

#endif
