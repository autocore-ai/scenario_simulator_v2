// Copyright 2015-2020 Autoware Foundation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SCENARIO_RUNNER__TYPE_TRAITS__IF_HAS_MEMBER_FUNCTION_START_HPP_
#define SCENARIO_RUNNER__TYPE_TRAITS__IF_HAS_MEMBER_FUNCTION_START_HPP_

#include <scenario_runner/type_traits/has_member_function_start.hpp>

namespace scenario_runner
{
inline namespace type_traits
{
template<typename T, typename = void>
struct IfHasMemberFunctionStart
{
  static constexpr void callIt(const T &) noexcept
  {}
};

template<typename T>
struct IfHasMemberFunctionStart<T, typename std::enable_if<HasMemberFunctionStart<T>::value>::type>
{
  static constexpr decltype(auto) callIt(T & then)
  {
    return then.start();
  }
};
}
}  // namespace scenario_runner

#endif  // SCENARIO_RUNNER__TYPE_TRAITS__IF_HAS_MEMBER_FUNCTION_START_HPP_