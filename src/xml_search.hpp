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
#ifndef AMI_DETAILS_XML_SEARCH_H_
#define AMI_DETAILS_XML_SEARCH_H_

#include <AMI/XML.hpp>
#include <functional>

namespace AMI {
namespace details {

class XMLSearcher : public XML::Visitor {
  public:
  using result_t = std::vector<const XML::Node*>;
  using predicate_t = std::function<bool(const XML::Node&)>;


  public:
  static XML::const_elements_t elements_by_name(const XML::Node& root, const std::string& name, bool only_one);
  static XML::const_attributes_t attributes_by_name(const XML::Node& root, const std::string& name, bool only_one);

  public:
  using Visitor::visit;
  XMLSearcher(void) = delete;

  virtual void visit(const XML::Manifest& node) override;
  virtual void visit(const XML::Namespace& node) override;
  virtual void visit(const XML::Element& node) override;
  virtual void visit(const XML::Attribute& node) override;
  virtual void visit(const XML::CData& node) override;

  bool try_insert(const XML::Node& node);

  result_t&& result(void);

  virtual ~XMLSearcher(void);

  private:
  XMLSearcher(const predicate_t& p, bool unique);

  result_t result_;
  predicate_t predicate_;
  bool unique_;


};


}
}

#endif
