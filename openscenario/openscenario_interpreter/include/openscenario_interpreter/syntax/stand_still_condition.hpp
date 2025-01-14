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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__STAND_STILL_CONDITION_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__STAND_STILL_CONDITION_HPP_

#include <openscenario_interpreter/procedure.hpp>
#include <openscenario_interpreter/syntax/rule.hpp>
#include <openscenario_interpreter/syntax/triggering_entities.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- StandStillCondition ----------------------------------------------------
 *
 *  <xsd:complexType name="StandStillCondition">
 *    <xsd:attribute name="duration" type="Double" use="required"/>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct StandStillCondition
{
  const Double duration;

  const Rule compare;

  const TriggeringEntities triggering_entities;

  std::vector<Double> last_checked_values;  // for description

  template <typename Node, typename Scope>
  explicit StandStillCondition(
    const Node & node, Scope & outer_scope, const TriggeringEntities & triggering_entities)
  : duration(readAttribute<Double>("duration", node, outer_scope)),
    compare(Rule::greaterThan),
    triggering_entities(triggering_entities),
    last_checked_values(triggering_entities.entity_refs.size(), Double::nan())
  {
  }

  auto description() const
  {
    std::stringstream description;

    description << triggering_entities.description() << "'s standstill time = ";

    print_to(description, last_checked_values);

    description << " " << compare << " " << duration << "?";

    return description.str();
  }

  auto evaluate()
  {
    last_checked_values.clear();

    return asBoolean(triggering_entities.apply([&](auto && triggering_entity) {
      last_checked_values.push_back(getStandStillDuration(triggering_entity));
      return compare(last_checked_values.back(), duration);
    }));
  }
};
}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__STAND_STILL_CONDITION_HPP_
