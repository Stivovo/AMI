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
#include <iostream>

#include <AMI/AMI.hpp>

using namespace AMI;
int main(int argc, char** argv) {
  std::cout << "Manifest Reader" << std::endl;

  Manifest manifest = Manifest::open(argv[1]);
  //std::cout << manifest.package_name() << std::endl;
  //std::cout << manifest.version_name() << std::endl;

  //for (Permission p : manifest.permissions()) {
  //  std::cout << p.fullname() << std::endl;
  //}

  //for (const Service& s : manifest.services()) {
  //  std::cout << s.fullname() << std::endl;
  //}

  //auto elements = manifest.find_elements("uses-permission");
  //for (auto e : elements) {
  //  std::cout << e->name() << std::endl;
  //}

  return 0;
}
