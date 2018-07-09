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
#include "pyAMI.hpp"
#include "AMI/XML.hpp"
#include <sstream>

using namespace AMI::XML;

#define VISIT(cls)                                                    \
  virtual void visit(const cls& v) override {                         \
    PYBIND11_OVERLOAD_NAME(void, Visitor, #cls, visit, std::cref(v)); \
  }

#define VISIT_IMPL(cls)                                               \
  virtual void visit(const cls& v) override {                         \
    PYBIND11_OVERLOAD_NAME(void, Visitor, #cls, visit, std::cref(v)); \
  }



class pyVisitor : public Visitor {
  public:
  using Visitor::Visitor;

  VISIT(Node)
  VISIT(Manifest)
  VISIT(Namespace)
  VISIT(Element)
  VISIT(Attribute)
  VISIT(CData)
};

void init_xml(py::module& m) {

  // =============================
  // Visitor
  // =============================
  py::class_<Visitor, pyVisitor>(m, "Visitor", "Visitor class to explore XML nodes")
    .def(py::init<>())
    .def("Node",
        static_cast<void(Visitor::*)(const Node&)>(&Visitor::visit))

    .def("Manifest",
        static_cast<void(Visitor::*)(const Manifest&)>(&Visitor::visit))

    .def("Namespace",
        static_cast<void(Visitor::*)(const Namespace&)>(&Visitor::visit))

    .def("Element",
        static_cast<void(Visitor::*)(const Element&)>(&Visitor::visit))

    .def("Attribute",
        static_cast<void(Visitor::*)(const Attribute&)>(&Visitor::visit))

    .def("CData",
        static_cast<void(Visitor::*)(const CData&)>(&Visitor::visit));


  // =============================
  // XML
  // =============================
  py::class_<Node>(m, "Node", "XML abstract node")
    .def_property_readonly("parent",
        static_cast<Node*(Node::*)(void)>(&Node::parent),
        py::return_value_policy::reference)

    .def_property_readonly("children",
        static_cast<Node::children_t(Node::*)(void)>(&Node::children))

    .def_property_readonly("children",
        static_cast<Node::children_t(Node::*)(void)>(&Node::children))

    .def("find_attributes",
        &Node::find_attributes,
        "name"_a, "only_one"_a = false)

    .def("find_elements",
        &Node::find_elements,
        "name"_a, "only_one"_a = false)


    .def("accept",
        &Node::accept)

    .def("__str__",
        [] (const Node& node) {
          std::ostringstream ss;
          node.dump(ss);
          return ss.str();
        });


  // Manifest
  py::class_<Manifest, Node>(m, "Manifest", "XML Manifest node");

  // Namespace
  py::class_<Namespace, Node>(m, "Namespace", "XML Namespace node")
    .def_property_readonly("prefix",
        static_cast<const std::string&(Namespace::*)(void) const>(&Namespace::prefix))

    .def_property_readonly("uri",
        static_cast<const std::string&(Namespace::*)(void) const>(&Namespace::uri));

  // Attribute
  py::class_<Attribute, Node>(m, "Attribute", "XML Attribute node")
    .def_property_readonly("name",
        static_cast<const std::string&(Attribute::*)(void) const>(&Attribute::name))

    .def_property_readonly("type",
        static_cast<Attribute::TYPES (Attribute::*)(void) const>(&Attribute::type))

    .def_property_readonly("value",
        [] (const Attribute& attribute) -> py::object {
          switch (attribute.type()) {
            case Attribute::TYPES::REFERENCE:
            case Attribute::TYPES::ATTRIBUTE:
            case Attribute::TYPES::DIMENSION:
            case Attribute::TYPES::DYNAMIC_REFERENCE:
            case Attribute::TYPES::DYNAMIC_ATTRIBUTE:
            case Attribute::TYPES::INT_DEC:
            case Attribute::TYPES::INT_HEX:
            case Attribute::TYPES::INT_BOOLEAN:
            case Attribute::TYPES::INT_COLOR_ARGB8:
            case Attribute::TYPES::INT_COLOR_RGB8:
            case Attribute::TYPES::INT_COLOR_ARGB4:
            case Attribute::TYPES::INT_COLOR_RGB4:
              {
                return py::int_(attribute.ivalue());
              }

            case Attribute::TYPES::STRING:
              {
                return py::str(attribute.svalue());
              }

            case Attribute::TYPES::FLOAT:
            case Attribute::TYPES::FRACTION:
              {
                return py::float_(attribute.fvalue());
              }

            case Attribute::TYPES::NONE:
            default:
              {
                return py::none{};
              }
          }
        });



  // Element
  py::class_<Element, Node>(m, "Element", "XML Element node")
    .def_property_readonly("name",
        static_cast<const std::string&(Element::*)(void) const>(&Element::name))

    .def_property_readonly("attributes",
        static_cast<attributes_t (Element::*)(void)>(&Element::attributes))

    .def("__getitem__",
        static_cast<Attribute*(Element::*)(const std::string&)>(&Element::operator[]),
        py::return_value_policy::reference);


  // CData
  py::class_<CData, Node>(m, "CData", "XML CData node")
    .def_property_readonly("cdata",
        static_cast<const std::string&(CData::*)(void) const>(&CData::cdata));


}
