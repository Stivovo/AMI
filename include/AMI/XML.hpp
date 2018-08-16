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
#ifndef AMI_XML_NODE_H_
#define AMI_XML_NODE_H_
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include <AMI/visibility.h>

namespace AMI {
namespace XML {

class Node;
class Manifest;
class Namespace;
class Element;
class Attribute;
class CData;


template< class T >
using add_pointer_t = typename std::add_pointer<T>::type;

template< class T >
using decay_t = typename std::decay<T>::type;

template< class T >
using add_const_t = typename std::add_const<T>::type;


using const_elements_t   = std::vector<const Element*>;

using const_attributes_t = std::vector<const Attribute*>;
using attributes_t       = std::vector<Attribute*>;

class AMI_API Visitor {
  public:
  Visitor(void);

  virtual void visit(const Node& node);
  virtual void visit(const Manifest& node);
  virtual void visit(const Namespace& node);
  virtual void visit(const Element& node);
  virtual void visit(const Attribute& node);
  virtual void visit(const CData& node);

  virtual ~Visitor(void);

};

class AMI_API Node {
  public:
  using children_const_t = std::vector<const Node*>;
  using children_t       = std::vector<Node*>;


  template<class T>
  using node_t = add_pointer_t<decay_t<T>>;

  template<class T>
  using node_const_t = add_pointer_t<add_const_t<decay_t<T>>>;

  public:
  Node(void);
  Node(const Node& other);
  virtual Node* clone(void) const = 0;

  const Node* parent(void) const;
  Node* parent(void);

  Node& add(Node* node);

  children_const_t children(void) const;
  children_t children(void);

  template<class T>
  bool is(void) const;

  template<class T>
  node_t<T> as(void);

  template<class T>
  node_const_t<T> as(void) const;

  const_attributes_t find_attributes(const std::string& name, bool only_one = false) const;

  const_elements_t find_elements(const std::string& name, bool only_one = false) const;

  virtual void accept(Visitor& visitor) const = 0;

  virtual std::ostream& dump(std::ostream& os) const;

  virtual ~Node(void);

  protected:

  Node* parent_{nullptr};
  std::vector<std::unique_ptr<Node>> children_;
};

class AMI_API Manifest : public Node {
  public:
  Manifest(void);
  Manifest(const Manifest&);

  virtual void accept(Visitor& visitor) const override;

  virtual Manifest* clone(void) const override;
  virtual ~Manifest(void);

};

class AMI_API Namespace : public Node {
  public:
  Namespace(void) = delete;
  Namespace(const std::string& prefix, const std::string& uri);
  Namespace(const Namespace&);
  virtual Namespace* clone(void) const override;

  virtual void accept(Visitor& visitor) const override;

  const std::string& prefix(void) const;
  const std::string& uri(void) const;

  virtual ~Namespace(void);

  private:
  std::string prefix_;
  std::string uri_;

};

class AMI_API Attribute : public Node {
  public:
  enum class TYPES : uint8_t {
    NONE              = 0x00,
    REFERENCE         = 0x01,
    ATTRIBUTE         = 0x02,
    STRING            = 0x03,
    FLOAT             = 0x04,
    DIMENSION         = 0x05,
    FRACTION          = 0x06,
    DYNAMIC_REFERENCE = 0x07,
    DYNAMIC_ATTRIBUTE = 0x08,
    INT_DEC           = 0x10,
    INT_HEX           = 0x11,
    INT_BOOLEAN       = 0x12,
    INT_COLOR_ARGB8   = 0x1c,
    INT_COLOR_RGB8    = 0x1d,
    INT_COLOR_ARGB4   = 0x1e,
    INT_COLOR_RGB4    = 0x1f,
  };

  public:
  Attribute(const Attribute&);
  Attribute(const std::string& name, const std::string& val, TYPES type);
  Attribute(const std::string& name, uint32_t val, TYPES type);
  Attribute(const std::string& name, float val, TYPES type);
  virtual Attribute* clone(void) const override;

  virtual void accept(Visitor& visitor) const override;

  const std::string& name(void) const;
  TYPES type(void) const;

  const std::string& svalue(void) const;
  uint32_t ivalue(void) const;
  float fvalue(void) const;

  virtual ~Attribute(void);

  protected:
  std::string name_;
  TYPES type_{TYPES::NONE};
  union {
    std::string* string_{nullptr};
    uint32_t* integer_;
    float* float_;
  };
};



class AMI_API Element : public Node {
  public:
  Element(const std::string& name, size_t nb_attributes = 0);
  Element(const Element& other);

  virtual void accept(Visitor& visitor) const override;
  const std::string& name(void) const;

  Attribute& add_attribute(const std::string& name, const std::string& value, Attribute::TYPES type);
  Attribute& add_attribute(const std::string& name, uint32_t value, Attribute::TYPES type);
  Attribute& add_attribute(const std::string& name, float value, Attribute::TYPES type);

  const_attributes_t attributes(void) const;
  attributes_t attributes(void);

  const Attribute* operator[](const std::string& attr_name) const;
  Attribute* operator[](const std::string& attr_name);

  const Attribute* get(const std::string& attr_name) const;
  Attribute* get(const std::string& attr_name);

  virtual Element* clone(void) const override;
  virtual ~Element(void);

  protected:
  using attributes_container_t = std::unordered_map<std::string, std::unique_ptr<Attribute>>;
  std::string name_;
  attributes_container_t attributes_;
};


class AMI_API CData : public Node {
  public:
  CData(const CData&);
  CData(const std::string& cdata);
  virtual CData* clone(void) const override;

  virtual void accept(Visitor& visitor) const override;

  const std::string& cdata(void) const;

  virtual ~CData(void);

  private:
  std::string cdata_;

};


template<class T>
bool Node::is(void) const {
  return typeid(*this) == typeid(T);
}

template<class T>
Node::node_t<T> Node::as(void) {
  return dynamic_cast<Node::node_t<T>>(this);
}

template<class T>
Node::node_const_t<T> Node::as(void) const {
  return dynamic_cast<Node::node_const_t<T>>(this);
}


}
}

#endif
