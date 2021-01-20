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

#ifndef SIMULATION_API__METRICS__TRAVELED_DISTANCE_METRIC_HPP_
#define SIMULATION_API__METRICS__TRAVELED_DISTANCE_METRIC_HPP_

#include <simulation_api/metrics/metric_base.hpp>

#include <string>

namespace metrics
{
class TraveledDistanceMetric : public MetricBase
{
public:
  explicit TraveledDistanceMetric(std::string target_entity);
  ~TraveledDistanceMetric() = default;
  void update() override;
  bool calculateFinished() override;
  nlohmann::json to_json();
  bool activateTrigger();

private:
  double traveled_distance;
};
}  // namespace metrics

#endif  // SIMULATION_API__METRICS__TRAVELED_DISTANCE_METRIC_HPP_
