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
#ifndef AMI_VISIBILITY_H_
#define AMI_VISIBILITY_H_

#if defined _WIN32 || defined __CYGWIN__
  #define AMI_HELPER_IMPORT __declspec(dllimport)
  #define AMI_HELPER_EXPORT __declspec(dllexport)
  #define AMI_HELPER_LOCAL
#else
  #define AMI_HELPER_IMPORT __attribute__ ((visibility ("default")))
  #define AMI_HELPER_EXPORT __attribute__ ((visibility ("default")))
  #define AMI_HELPER_LOCAL  __attribute__ ((visibility ("hidden")))
#endif

#if defined(AMI_STATIC)
  #define AMI_API
  #define AMI_LOCAL
#elif defined(AMI_EXPORTS)
  #define AMI_API   AMI_HELPER_EXPORT
  #define AMI_LOCAL AMI_HELPER_LOCAL
#elif defined(AMI_IMPORT)
  #define AMI_API   AMI_HELPER_IMPORT
  #define AMI_LOCAL AMI_HELPER_LOCAL
#else
  #define AMI_API
  #define AMI_LOCAL AMI_HELPER_LOCAL
#endif

#endif
