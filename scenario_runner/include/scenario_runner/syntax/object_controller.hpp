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

#ifndef SCENARIO_RUNNER__SYNTAX__OBJECT_CONTROLLER_HPP_
#define SCENARIO_RUNNER__SYNTAX__OBJECT_CONTROLLER_HPP_

#include <scenario_runner/reader/element.hpp>

#include <utility>

namespace scenario_runner
{
inline namespace syntax
{
/* ==== ObjectController =====================================================
 *
 * <xsd:complexType name="ObjectController">
 *   <xsd:choice>
 *     <xsd:element name="CatalogReference" type="CatalogReference"/>
 *     <xsd:element name="Controller" type="Controller"/>
 *   </xsd:choice>
 * </xsd:complexType>
 *
 * ======================================================================== */
struct ObjectController
  : public Object
{
  template<typename Node, typename Scope>
  explicit ObjectController(const Node & node, Scope &)
  {
    callWithElements(node, "CatalogReference", 0, 1, THROW_UNSUPPORTED_ERROR(node));
    callWithElements(node, "Controller", 0, 1, THROW_UNSUPPORTED_ERROR(node));
  }
};
}
}  // namespace scenario_runner

#endif  // SCENARIO_RUNNER__SYNTAX__OBJECT_CONTROLLER_HPP_
