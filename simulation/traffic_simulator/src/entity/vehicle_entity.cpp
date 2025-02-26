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

#include <quaternion_operation/quaternion_operation.h>

#include <boost/algorithm/clamp.hpp>
#include <memory>
#include <openscenario_msgs/msg/vehicle_parameters.hpp>
#include <string>
#include <traffic_simulator/entity/vehicle_entity.hpp>
#include <vector>

namespace traffic_simulator
{
namespace entity
{
VehicleEntity::VehicleEntity(
  const std::string & name, const openscenario_msgs::msg::VehicleParameters & params)
: EntityBase(params.vehicle_category, name),
  parameters(params),
  tree_ptr_(std::make_shared<entity_behavior::vehicle::BehaviorTree>())
{
  entity_type_.type = openscenario_msgs::msg::EntityType::VEHICLE;
  tree_ptr_->setValueToBlackBoard("vehicle_parameters", parameters);
}

void VehicleEntity::requestAssignRoute(
  const std::vector<openscenario_msgs::msg::LaneletPose> & waypoints)
{
  if (status_ and status_->lanelet_pose_valid) {
    route_planner_ptr_->getRouteLanelets(status_->lanelet_pose, waypoints);
  }
}

void VehicleEntity::requestAssignRoute(const std::vector<geometry_msgs::msg::Pose> & waypoints)
{
  std::vector<openscenario_msgs::msg::LaneletPose> route;
  for (const auto & waypoint : waypoints) {
    const auto lanelet_waypoint = hdmap_utils_ptr_->toLaneletPose(waypoint);
    if (lanelet_waypoint) {
      route.emplace_back(lanelet_waypoint.get());
    } else {
      THROW_SEMANTIC_ERROR("Waypoint of pedestrian entity should be on lane.");
    }
  }
  requestAssignRoute(route);
}

void VehicleEntity::requestAcquirePosition(const openscenario_msgs::msg::LaneletPose & lanelet_pose)
{
  if (status_ and status_->lanelet_pose_valid) {
    route_planner_ptr_->getRouteLanelets(status_->lanelet_pose, lanelet_pose);
  }
}

void VehicleEntity::requestAcquirePosition(const geometry_msgs::msg::Pose & map_pose)
{
  const auto lanelet_pose = hdmap_utils_ptr_->toLaneletPose(map_pose);
  if (lanelet_pose) {
    requestAcquirePosition(lanelet_pose.get());
  } else {
    THROW_SEMANTIC_ERROR("Goal of the vehicle entity should be on lane.");
  }
}

void VehicleEntity::requestLaneChange(const std::int64_t to_lanelet_id)
{
  tree_ptr_->setRequest("lane_change");
  tree_ptr_->setValueToBlackBoard("to_lanelet_id", to_lanelet_id);
}

void VehicleEntity::cancelRequest() { tree_ptr_->setRequest("none"); }

void VehicleEntity::setTargetSpeed(double target_speed, bool continuous)
{
  target_speed_planner_.setTargetSpeed(target_speed, continuous);
}

void VehicleEntity::onUpdate(double current_time, double step_time)
{
  EntityBase::onUpdate(current_time, step_time);
  if (!status_) {
    return;
  }
  if (current_time < 0) {
    updateEntityStatusTimestamp(current_time);
  } else {
    tree_ptr_->setValueToBlackBoard("other_entity_status", other_status_);
    tree_ptr_->setValueToBlackBoard("entity_type_list", entity_type_list_);
    tree_ptr_->setValueToBlackBoard("entity_status", status_.get());
    target_speed_planner_.update(status_->action_status.twist.linear.x);
    tree_ptr_->setValueToBlackBoard("target_speed", target_speed_planner_.getTargetSpeed());
    if (status_->lanelet_pose_valid) {
      tree_ptr_->setValueToBlackBoard(
        "route_lanelets", route_planner_ptr_->getRouteLanelets(status_->lanelet_pose));
    } else {
      std::vector<std::int64_t> empty = {};
      tree_ptr_->setValueToBlackBoard("route_lanelets", empty);
    }
    tree_ptr_->tick(current_time, step_time);
    auto status_updated = tree_ptr_->getUpdatedStatus();
    if (status_updated.lanelet_pose_valid) {
      auto following_lanelets =
        hdmap_utils_ptr_->getFollowingLanelets(status_updated.lanelet_pose.lanelet_id);
      auto l = hdmap_utils_ptr_->getLaneletLength(status_updated.lanelet_pose.lanelet_id);
      if (following_lanelets.size() == 1 && l <= status_updated.lanelet_pose.s) {
        stopAtEndOfRoad();
        return;
      }
    }
    if (!status_) {
      linear_jerk_ = 0;
    } else {
      linear_jerk_ =
        (status_updated.action_status.accel.linear.x - status_->action_status.accel.linear.x) /
        step_time;
    }
    setStatus(status_updated);
    updateStandStillDuration(step_time);
  }
}
}  // namespace entity
}  // namespace traffic_simulator
