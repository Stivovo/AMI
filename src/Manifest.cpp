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

#include <algorithm>

namespace AMI {

Manifest::~Manifest(void) = default;

Manifest Manifest::open(const std::string& path) {
  std::unique_ptr<details::AndroidManifestImpl> impl{details::AndroidManifestImpl::open(path)};
  return std::move(impl);
}


Manifest::Manifest(const Manifest& other) :
  pimpl_{nullptr}
{
  if (other) {
    this->pimpl_ = std::unique_ptr<details::AndroidManifestImpl>{
      new details::AndroidManifestImpl{*other.pimpl_}
    };
  }
}


std::string Manifest::package_name(void) const {
  CHECK(static_cast<bool>(this->pimpl_));
  const XML::Element* manifest = this->pimpl_->manifest().find_elements("manifest", /* only_one */ true).back();
  XML::const_attributes_t attributes = manifest->find_attributes("package", /* only_one */ true);
  if (attributes.size() == 0) {
    return "";
  }
  const XML::Attribute* package = attributes[0];
  CHECK_EQ(package->type(), XML::Attribute::TYPES::STRING);
  return package->svalue();

}

std::string Manifest::version_name(void) const {
  CHECK(static_cast<bool>(this->pimpl_));
  const XML::Element* manifest = this->pimpl_->manifest().find_elements("manifest", /* only_one */ true).back();
  XML::const_attributes_t attributes = manifest->find_attributes("android:versionName", /* only_one */ true);
  if (attributes.size() == 0) {
    return "";
  }

  // Only one version defined
  CHECK_EQ(attributes.size(), 1);

  // with string type
  CHECK_EQ(attributes[0]->type(), XML::Attribute::TYPES::STRING);
  return attributes[0]->svalue();
}

size_t Manifest::version_code(void) const {
  CHECK(static_cast<bool>(this->pimpl_));
  const XML::Element* manifest = this->pimpl_->manifest().find_elements("manifest", /* only_one */ true).back();
  XML::const_attributes_t attributes = manifest->find_attributes("android:versionCode", /* only_one */ true);
  if (attributes.size() == 0) {
    return 0;
  }

  // Only one version defined
  CHECK_EQ(attributes.size(), 1);

  // with string type
  //CHECK_EQ(attributes[0]->type(), XML::Attribute::TYPES::);
  return attributes[0]->ivalue();
}


Manifest::permission_list_t Manifest::permissions(void) const {
  XML::const_elements_t permissions = this->pimpl_->manifest().find_elements("uses-permission");
  Manifest::permission_list_t permissions_obj;
  permissions_obj.reserve(permissions.size());

  std::transform(
      std::begin(permissions),
      std::end(permissions),
      std::back_inserter(permissions_obj),
      [] (const XML::Element* e) -> const XML::Element& {
        return *e;
      });
  return permissions_obj;
}


Manifest::services_list_t Manifest::services(void) const {
  XML::const_elements_t services = this->pimpl_->manifest().find_elements("service");
  Manifest::services_list_t services_obj;
  services_obj.reserve(services.size());

  std::transform(
      std::begin(services),
      std::end(services),
      std::back_inserter(services_obj),
      [] (const XML::Element* e) -> const XML::Element& {
        return *e;
      });
  return services_obj;

}

Manifest::activities_list_t Manifest::activities(void) const {
  XML::const_elements_t activities = this->pimpl_->manifest().find_elements("activity");
  Manifest::activities_list_t activity_obj;
  activity_obj.reserve(activities.size());

  std::transform(
      std::begin(activities),
      std::end(activities),
      std::back_inserter(activity_obj),
      [] (const XML::Element* e) -> const XML::Element& {
        return *e;
      });
  return activity_obj;
}

Manifest::receivers_list_t Manifest::receivers(void) const {
  XML::const_elements_t br = this->pimpl_->manifest().find_elements("receiver");
  Manifest::receivers_list_t br_obj;
  br_obj.reserve(br.size());

  std::transform(
      std::begin(br),
      std::end(br),
      std::back_inserter(br_obj),
      [] (const XML::Element* e) -> const XML::Element& {
        return *e;
      });
  return br_obj;
}


Manifest& Manifest::operator=(const Manifest& other) {
  if (*this and this != &other) {
    this->pimpl_.reset(new details::AndroidManifestImpl{*other.pimpl_});
  }
  return *this;
}

Manifest::Manifest(std::unique_ptr<details::AndroidManifestImpl>&& impl) :
  pimpl_{std::move(impl)}
{}


XML::Manifest& Manifest::xml(void) {
  return this->pimpl_->manifest();
}

const XML::Manifest& Manifest::xml(void) const {
  return this->pimpl_->manifest();
}

Manifest::operator bool() const {
  return this->pimpl_ != nullptr;
}

std::ostream& operator<<(std::ostream& os, const Manifest& m) {
  static const std::string SPACE = "   ";
  Manifest::activities_list_t activities  = m.activities();
  Manifest::services_list_t services      = m.services();
  Manifest::permission_list_t permissions = m.permissions();
  Manifest::receivers_list_t receivers    = m.receivers();
  os << m.package_name() << " - " << m.version_name() << std::endl;

  os << "Permissions (" << std::dec << permissions.size() << "):" << std::endl;
  for (const Permission& p : permissions) {
    os << SPACE << p << std::endl;
  }

  os << std::endl;

  os << "Activities (" << std::dec << activities.size() << "):" << std::endl;
  for (const Activity& a : activities) {
    os << SPACE << a << std::endl;
  }

  os << std::endl;

  os << "Services (" << std::dec << services.size() << "):" << std::endl;
  for (const Service& s : services) {
    os << SPACE << s << std::endl;
  }

  os << std::endl;


  os << "Broadcast Receivers (" << std::dec << receivers.size() << "):" << std::endl;
  for (const BroadcastReceiver& br : receivers) {
    os << SPACE << br << std::endl;
  }


  return os;
}

} // Namespace AMI
