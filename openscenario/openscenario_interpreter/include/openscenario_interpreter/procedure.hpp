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

#ifndef OPENSCENARIO_INTERPRETER__PROCEDURE_HPP_
#define OPENSCENARIO_INTERPRETER__PROCEDURE_HPP_

#include <simulation_api/api/api.hpp>
#include <openscenario_interpreter/error.hpp>

#include <limits>
#include <memory>
#include <utility>

namespace openscenario_interpreter
{
extern scenario_simulator::API & connection;

// static struct Connector
// {
//   Connector();
//   ~Connector();
// } connector;

template<typename ... Ts>
decltype(auto) connect(Ts && ... xs)
{
  new (&connection) scenario_simulator::API(std::forward<decltype(xs)>(xs)...);
  return connection;
}

template<typename ... Ts>
decltype(auto) initialize(Ts && ... xs)
{
  return connection.simulation->initialize(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) spawn(Ts && ... xs)
{
  return connection.entity->spawn(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) requestLaneChange(Ts && ... xs)
{
  return connection.entity->requestLaneChange(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) isInLanelet(Ts && ... xs)
{
  return connection.entity->isInLanelet(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) setTargetSpeed(Ts && ... xs)
{
  return connection.entity->setTargetSpeed(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) getEntityStatus(Ts && ... xs) try {
  return connection.entity->getEntityStatus(std::forward<decltype(xs)>(xs)...);
} catch (const simulation_api::SimulationRuntimeError & error) {
  std::stringstream ss {};
  ss << error.what() << ".\n";
  ss << "Possible causes:\n";
  ss << "  (1) The position of the corresponding entity is not specified by Teleport Action";
  throw SemanticError(ss.str());
}

template<typename ... Ts>
decltype(auto) setEntityStatus(Ts && ... xs)
{
  return connection.entity->setEntityStatus(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) getCurrentTime(Ts && ... xs)
{
  return connection.simulation->getCurrentTime(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) isReachedPosition(Ts && ... xs)
{
  return connection.entity->reachPosition(std::forward<decltype(xs)>(xs)...);
}

// template<typename ... Ts>
// decltype(auto) getRelativeDistance(Ts && ... xs)
// {
//   return connection.entity->getRelativeDistance(std::forward<decltype(xs)>(xs)...);
// }

template<typename ... Ts>
decltype(auto) getRelativePose(Ts && ... xs)
{
  return connection.entity->getRelativePose(std::forward<decltype(xs)>(xs)...);
}

template<typename ... Ts>
decltype(auto) updateFrame(Ts && ... xs)
{
  return connection.simulation->updateFrame(std::forward<decltype(xs)>(xs)...);
}

// template <typename... Ts>
// auto getDistanceAlongRoute(Ts&&... xs)
// {
//   if (const auto result {
//   connection->entity->getLongitudinalDistance(std::forward<decltype(xs)>(xs)...) })
//   {
//     return *result;
//   }
//   else
//   {
//     using value_type = typename std::decay<decltype(result)>::type::value_type;
//     return std::numeric_limits<value_type>::infinity();
//   }
// }

template<typename ... Ts>
auto getTimeHeadway(Ts && ... xs)
{
  const auto result {
    connection.entity->getTimeHeadway(std::forward<decltype(xs)>(xs)...)
  };
  if (result) {
    return *result;
  } else {
    using value_type = typename std::decay<decltype(result)>::type::value_type;
    return std::numeric_limits<value_type>::quiet_NaN();
  }
}

template<typename ... Ts>
decltype(auto) requestAcquirePosition(Ts && ... xs)
{
  return connection.entity->requestAcquirePosition(std::forward<decltype(xs)>(xs)...);
}
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__PROCEDURE_HPP_