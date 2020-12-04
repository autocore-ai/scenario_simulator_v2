// Copyright 2015-2020 Tier IV, Inc. All rights reserved.
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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__TELEPORT_ACTION_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__TELEPORT_ACTION_HPP_

#include <openscenario_interpreter/procedure.hpp>
#include <openscenario_interpreter/syntax/position.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- TeleportAction ---------------------------------------------------------
 *
 * <xsd:complexType name="TeleportAction">
 *   <xsd:sequence>
 *     <xsd:element name="Position" type="Position"/>
 *   </xsd:sequence>
 * </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct TeleportAction
{
  Scope inner_scope;

  const Position position;

  template
  <
    typename Node
  >
  explicit TeleportAction(const Node & node, Scope & outer_scope)
  : inner_scope(outer_scope),
    position(
      readElement<Position>("Position", node, inner_scope))
  {}

  void start() const
  {
    if (position.is<LanePosition>()) {
      const simulation_api::entity::EntityStatus status {
        getCurrentTime(),
        Integer(position.as<LanePosition>().lane_id),
        position.as<LanePosition>().s,
        position.as<LanePosition>().offset,
        position.as<LanePosition>().orientation,
        geometry_msgs::msg::Twist(),
        geometry_msgs::msg::Accel()
      };
      for (const auto & each : inner_scope.actors) {
        setEntityStatus(each, status);
      }
    } else {
      THROW(ImplementationFault);
    }
  }

  static constexpr auto accomplished() noexcept
  {
    return true;
  }
};
}
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__TELEPORT_ACTION_HPP_
