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
#ifndef AMI_MACROS_H_
#define AMI_MACROS_H_
#include <iostream>

#define ABORT_AFTER_LOG_FATAL

#define LIKELY(x)       __builtin_expect((x),1)
#define UNLIKELY(x)     __builtin_expect((x),0)

#define ABORT_AFTER_LOG_EXPR_IF(c, x) (x)
#define MUST_LOG_MESSAGE(severity) false

#define ABORT_AFTER_LOG_FATAL_EXPR(x) ABORT_AFTER_LOG_EXPR_IF(true, x)

#define CHECK(x)                                                                 \
  LIKELY((x)) || ABORT_AFTER_LOG_FATAL_EXPR(false) ||                            \
      std::cerr << __FILE__ << ":" << __LINE__                                   \
                << " Check failed: " #x << " ";


#define CHECK_OP(LHS, RHS, OP)                                                   \
  LIKELY((LHS) OP (RHS)) || ABORT_AFTER_LOG_FATAL_EXPR(false) ||                            \
      std::cerr << __FILE__ << ":" << __LINE__                                   \
                << " Check failed: " #LHS << " " << #OP << " " << #RHS << std::endl;

#define CHECK_EQ(x, y) CHECK_OP(x, y, == )
#define CHECK_NE(x, y) CHECK_OP(x, y, != )
#define CHECK_LE(x, y) CHECK_OP(x, y, <= )
#define CHECK_LT(x, y) CHECK_OP(x, y, < )
#define CHECK_GE(x, y) CHECK_OP(x, y, >= )
#define CHECK_GT(x, y) CHECK_OP(x, y, > )

#endif
