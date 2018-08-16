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
#ifndef AMI_ANDROID_MANIFEST_H_
#define AMI_ANDROID_MANIFEST_H_
#include <memory>
#include <string>
#include <vector>

#include <AMI/visibility.h>
#include <AMI/XML.hpp>


namespace AMI {
namespace details {
class AndroidManifestImpl;
}


class AMI_API Permission {
  public:
  Permission(void) = delete;
  Permission(const XML::Element& e);

  Permission(const Permission&);
  Permission& operator=(const Permission&);

  std::string fullname(void) const;

  AMI_API friend std::ostream& operator<<(std::ostream& os, const Permission& m);

  private:
  XML::Element* element_{nullptr};
};

class AMI_API Activity {
  public:
  Activity(void) = delete;
  Activity(const XML::Element& e);

  Activity(const Activity&);
  Activity& operator=(const Activity&);

  std::string cls(void) const;

  AMI_API friend std::ostream& operator<<(std::ostream& os, const Activity& m);

  private:
  XML::Element* element_{nullptr};
};


class AMI_API Service {
  public:
  using intent_actions_t    = std::vector<std::string>;
  using intent_categories_t = std::vector<std::string>;

  public:
  Service(void) = delete;
  Service(const XML::Element& e);

  Service(const Service&);
  Service& operator=(const Service&);

  std::string cls(void) const;

  bool is_exported(void) const;
  bool is_enabled(void) const;
  bool is_isolated(void) const;
  size_t priority(void) const;

  //Permission permission(void) const;

  intent_actions_t    actions(void) const;
  intent_categories_t categories(void) const;

  AMI_API friend std::ostream& operator<<(std::ostream& os, const Service& m);

  private:
  XML::Element* element_{nullptr};
};


class AMI_API Application {

  public:
  Application(void);
  Application(const XML::Element& e);

  Application(const Application&);
  Application& operator=(const Application&);

  std::string cls(void) const;
  bool is_debuggable(void) const;
  std::string process(void) const;
  std::string backup_agent(void) const;

  operator bool() const;

  AMI_API friend std::ostream& operator<<(std::ostream& os, const Application& m);

  private:
  XML::Element* element_{nullptr};
};

class AMI_API BroadcastReceiver {
  public:
  BroadcastReceiver(void) = delete;
  BroadcastReceiver(const XML::Element& e);

  BroadcastReceiver(const BroadcastReceiver&);
  BroadcastReceiver& operator=(const BroadcastReceiver&);

  std::string cls(void) const;

  AMI_API friend std::ostream& operator<<(std::ostream& os, const BroadcastReceiver& m);

  private:
  XML::Element* element_{nullptr};
};



class AMI_API Manifest {
  public:
  using permission_list_t = std::vector<Permission>;
  using services_list_t   = std::vector<Service>;
  using activities_list_t = std::vector<Activity>;
  using receivers_list_t  = std::vector<BroadcastReceiver>;

  public:
  Manifest(void) = delete;
  Manifest(const Manifest& other);
  Manifest& operator=(const Manifest& other);

  static Manifest open(const std::string& path);

  std::string package_name(void) const;
  std::string version_name(void) const;
  size_t      version_code(void) const;

  operator bool() const;

  permission_list_t permissions(void) const;
  services_list_t   services(void) const;
  activities_list_t activities(void) const;
  receivers_list_t  receivers(void) const;
  Application       application(void) const;

  XML::Manifest&       xml(void);
  const XML::Manifest& xml(void) const;

  AMI_API friend std::ostream& operator<<(std::ostream& os, const Manifest& m);

  ~Manifest(void);

  private:
  Manifest(std::unique_ptr<details::AndroidManifestImpl>&& impl);
  std::unique_ptr<details::AndroidManifestImpl> pimpl_;

};

} // Namespace AMI

#endif
