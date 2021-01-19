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

#include <simulation_api/helper/helper.hpp>

#include <quaternion_operation/quaternion_operation.h>

namespace simulation_api
{
namespace helper
{
openscenario_msgs::msg::ActionStatus constractActionStatus(
  double linear_vel,
  double angular_vel,
  double linear_accel,
  double angular_accel)
{
  openscenario_msgs::msg::ActionStatus status;
  geometry_msgs::msg::Twist twist;
  twist.linear.x = linear_vel;
  twist.angular.z = angular_vel;
  geometry_msgs::msg::Accel accel;
  accel.linear.x = linear_accel;
  accel.angular.z = angular_accel;
  status.twist = twist;
  status.accel = accel;
  return status;
}

openscenario_msgs::msg::LaneletPose constractLaneletPose(
  std::int64_t lanelet_id, double s,
  double offset, double roll,
  double pitch, double yaw)
{
  openscenario_msgs::msg::LaneletPose lanelet_pose;
  lanelet_pose.lanelet_id = lanelet_id;
  lanelet_pose.s = s;
  lanelet_pose.offset = offset;
  lanelet_pose.rpy.x = roll;
  lanelet_pose.rpy.y = pitch;
  lanelet_pose.rpy.z = yaw;
  return lanelet_pose;
}

geometry_msgs::msg::Vector3 constractRPY(double roll, double pitch, double yaw)
{
  geometry_msgs::msg::Vector3 rpy;
  rpy.x = roll;
  rpy.y = pitch;
  rpy.z = yaw;
  return rpy;
}

geometry_msgs::msg::Vector3 constractRPYfronQuaternion(geometry_msgs::msg::Quaternion quaternion)
{
  return quaternion_operation::convertQuaternionToEulerAngle(quaternion);
}

geometry_msgs::msg::Pose constractPose(
  double x, double y, double z, double roll, double pitch,
  double yaw)
{
  geometry_msgs::msg::Pose pose;
  pose.position.x = x;
  pose.position.y = y;
  pose.position.z = z;
  pose.orientation =
    quaternion_operation::convertEulerAngleToQuaternion(constractRPY(roll, pitch, yaw));
  return pose;
}

std::ostream & operator<<(std::ostream & os, const openscenario_msgs::msg::LaneletPose & ll_pose)
{
  os << "lanelet id : " << ll_pose.lanelet_id << "\ns : " << ll_pose.s;
  return os;
}

}  // namespace helper
}  // namespace simulation_api