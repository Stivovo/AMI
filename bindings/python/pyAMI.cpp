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
#include "AMI/AMI.hpp"
//#include "AMI/version.h"
#include <sstream>

py::module ami_module("pyAMI", "AMI Python API");

using namespace AMI;


PYBIND11_MODULE(pyAMI, ami_module) {

  //ami_module.attr("__version__") = py::str(AMI_VERSION);

  py::class_<Service>(ami_module, "Service", "Android Service representation")
    .def_property_readonly("cls",
        &Service::cls)

    .def_property_readonly("is_exported",
        &Service::is_exported)

    .def_property_readonly("priority",
        &Service::priority)

    .def("__str__",
        [] (const Service& service) {
          std::ostringstream oss;
          oss << service;
          return oss.str();
        });


  py::class_<Permission>(ami_module, "Permission", "Android Permission representation")

    .def_property_readonly("fullname",
        &Permission::fullname)

    .def("__str__",
        [] (const Permission& perm) {
          std::ostringstream oss;
          oss << perm;
          return oss.str();
        });

  py::class_<Activity>(ami_module, "Activity", "Android Activity representation")
    .def_property_readonly("cls",
        &Activity::cls)

    .def("__str__",
        [] (const Activity& activity) {
          std::ostringstream oss;
          oss << activity;
          return oss.str();
        });


  py::class_<BroadcastReceiver>(ami_module, "BroadcastReceiver", "Android BroadcastReceiver representation")
    .def_property_readonly("cls",
        &BroadcastReceiver::cls)

    .def("__str__",
        [] (const BroadcastReceiver& br) {
          std::ostringstream oss;
          oss << br;
          return oss.str();
        });


  py::class_<Manifest>(ami_module, "Manifest", "Android Manifest representation")
    .def_static("open",
        [] (const std::string& path) -> py::object {
          Manifest manifest = Manifest::open(path);
          if (manifest) {
            return py::cast(manifest);
          } else {
            return py::none();
          }
        })

    .def_property_readonly("package_name",
        &Manifest::package_name)

    .def_property_readonly("version_name",
        &Manifest::version_name)

    .def_property_readonly("version_code",
        &Manifest::version_code)

    .def_property_readonly("permissions",
        &Manifest::permissions)

    .def_property_readonly("services",
        &Manifest::services)

    .def_property_readonly("activities",
        &Manifest::activities)

    .def_property_readonly("receivers",
        &Manifest::receivers)

    .def("__bool__",
        [] (const Manifest& manifest) {
          return static_cast<bool>(manifest);
        })

    .def_property_readonly("xml",
        static_cast<XML::Manifest&(Manifest::*)(void)>(&Manifest::xml),
        py::return_value_policy::reference)

    .def("__str__",
        [] (const Manifest& m) {
          std::ostringstream ss;
          ss << m;
          return ss.str();
        });

  py::module xml_module = ami_module.def_submodule("XML", "XML API");
  init_xml(xml_module);

}
