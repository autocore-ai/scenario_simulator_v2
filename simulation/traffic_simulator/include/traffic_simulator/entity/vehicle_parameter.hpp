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

#ifndef TRAFFIC_SIMULATOR__ENTITY__VEHICLE_PARAMETER_HPP_
#define TRAFFIC_SIMULATOR__ENTITY__VEHICLE_PARAMETER_HPP_

// headers in pugixml
#include <openscenario_msgs.pb.h>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <openscenario_msgs/msg/bounding_box.hpp>
#include <openscenario_msgs/msg/vehicle_parameters.hpp>
#include <pugixml.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sstream>
#include <string>

namespace traffic_simulator
{
namespace entity
{
struct Performance
{
  explicit Performance(const pugi::xml_node & xml)
  : max_speed(xml.child("Performance").attribute("maxSpeed").as_double()),
    max_acceleration(xml.child("Performance").attribute("maxAcceleration").as_double()),
    max_deceleration(xml.child("Performance").attribute("maxDeceleration").as_double())
  {
  }
  Performance(double max_speed, double max_acceleration, double max_deceleration)
  : max_speed(max_speed), max_acceleration(max_acceleration), max_deceleration(max_deceleration)
  {
  }
  const double max_speed;
  const double max_acceleration;
  const double max_deceleration;
  const openscenario_msgs::msg::Performance toRosMsg() const
  {
    openscenario_msgs::msg::Performance ret;
    ret.max_speed = max_speed;
    ret.max_acceleration = max_acceleration;
    ret.max_deceleration = max_deceleration;
    return ret;
  }
};

struct Center
{
  explicit Center(const pugi::xml_node & xml)
  : x(xml.child("BoundingBox").child("Center").attribute("x").as_double()),
    y(xml.child("BoundingBox").child("Center").attribute("y").as_double()),
    z(xml.child("BoundingBox").child("Center").attribute("z").as_double())
  {
  }
  Center(double x, double y, double z) : x(x), y(y), z(z) {}
  const double x;
  const double y;
  const double z;
};

struct Dimensions
{
  explicit Dimensions(const pugi::xml_node & xml)
  : width(xml.child("BoundingBox").child("Dimensions").attribute("width").as_double()),
    length(xml.child("BoundingBox").child("Dimensions").attribute("length").as_double()),
    height(xml.child("BoundingBox").child("Dimensions").attribute("height").as_double())
  {
  }
  Dimensions(double width, double length, double height)
  : width(width), length(length), height(height)
  {
  }
  const double width;
  const double length;
  const double height;
};

struct BoundingBox
{
  explicit BoundingBox(const pugi::xml_node & xml) : center(xml), dimensions(xml) {}
  BoundingBox(Center center, Dimensions dimensions) : center(center), dimensions(dimensions) {}
  const Center center;
  const Dimensions dimensions;
  const openscenario_msgs::msg::BoundingBox toRosMsg() const
  {
    openscenario_msgs::msg::BoundingBox ret;
    ret.center.x = center.x;
    ret.center.y = center.y;
    ret.center.z = center.z;
    ret.dimensions.x = dimensions.length;
    ret.dimensions.y = dimensions.width;
    ret.dimensions.z = dimensions.height;
    return ret;
  }
};

struct Axle
{
  explicit Axle(const pugi::xml_node & xml)
  : max_steering(xml.attribute("maxSteering").as_double()),
    wheel_diameter(xml.attribute("wheelDiameter").as_double()),
    track_width(xml.attribute("trackWidth").as_double()),
    position_x(xml.attribute("positionX").as_double()),
    position_z(xml.attribute("positionZ").as_double())
  {
  }
  Axle(
    double max_steering, double wheel_diameter, double track_width, double position_x,
    double position_z)
  : max_steering(max_steering),
    wheel_diameter(wheel_diameter),
    track_width(track_width),
    position_x(position_x),
    position_z(position_z)
  {
  }
  const double max_steering;
  const double wheel_diameter;
  const double track_width;
  const double position_x;
  const double position_z;
  const openscenario_msgs::msg::Axle toRosMsg() const
  {
    openscenario_msgs::msg::Axle ret;
    ret.max_steering = max_steering;
    ret.wheel_diameter = wheel_diameter;
    ret.track_width = track_width;
    ret.position_x = position_x;
    ret.position_z = position_z;
    return ret;
  }
};

struct Axles
{
  explicit Axles(const pugi::xml_node & xml)
  : front_axle(xml.child("Axles").child("FrontAxle")),
    rear_axle(xml.child("Axles").child("RearAxle"))
  {
  }
  Axles(Axle front_axle, Axle rear_axle) : front_axle(front_axle), rear_axle(rear_axle) {}
  const Axle front_axle;
  const Axle rear_axle;
  const openscenario_msgs::msg::Axles toRosMsg() const
  {
    openscenario_msgs::msg::Axles ret;
    ret.front_axle = front_axle.toRosMsg();
    ret.rear_axle = rear_axle.toRosMsg();
    return ret;
  }
};

struct VehicleParameters
{
  explicit VehicleParameters(const pugi::xml_node & xml)
  : performance(xml.child("Vehicle")),
    bounding_box(xml.child("Vehicle")),
    axles(xml.child("Vehicle")),
    name(xml.attribute("name").as_string()),
    vehicle_category(xml.attribute("vehicleCategory").as_string())
  {
  }
  VehicleParameters(
    std::string name, std::string vehicle_category, Performance performance,
    BoundingBox bounding_box, Axles axles)
  : performance(performance),
    bounding_box(bounding_box),
    axles(axles),
    name(name),
    vehicle_category(vehicle_category)
  {
  }
  const Performance performance;
  const BoundingBox bounding_box;
  const Axles axles;
  const std::string name;
  const std::string vehicle_category;

  const openscenario_msgs::msg::VehicleParameters toRosMsg()
  {
    openscenario_msgs::msg::VehicleParameters ret;
    ret.name = name;
    ret.vehicle_category = vehicle_category;
    ret.bounding_box = bounding_box.toRosMsg();
    ret.axles = axles.toRosMsg();
    ret.performance = performance.toRosMsg();
    return ret;
  }

  std::string toXml() const
  {
    using boost::property_tree::ptree;
    ptree pt;
    ptree & vehicle_tree = pt.add("Vehicle", "");
    vehicle_tree.put("<xmlattr>.name", name);
    vehicle_tree.put("<xmlattr>.vehicleCategory", vehicle_category);
    ptree & performance_tree = vehicle_tree.add("Performance", "");
    performance_tree.put("<xmlattr>.maxSpeed", performance.max_speed);
    performance_tree.put("<xmlattr>.maxAcceleration", performance.max_acceleration);
    performance_tree.put("<xmlattr>.maxDeceleration", performance.max_deceleration);
    ptree & center_tree = vehicle_tree.add("BoundingBox.Center", "");
    center_tree.put("<xmlattr>.x", bounding_box.center.x);
    center_tree.put("<xmlattr>.y", bounding_box.center.y);
    center_tree.put("<xmlattr>.z", bounding_box.center.z);
    ptree & dimensions_tree = vehicle_tree.add("BoundingBox.Dimensions", "");
    dimensions_tree.put("<xmlattr>.width", bounding_box.dimensions.width);
    dimensions_tree.put("<xmlattr>.length", bounding_box.dimensions.length);
    dimensions_tree.put("<xmlattr>.height", bounding_box.dimensions.height);
    ptree & front_axle_tree = vehicle_tree.add("Axles.FrontAxle", "");
    front_axle_tree.put("<xmlattr>.maxSteering", axles.front_axle.max_steering);
    front_axle_tree.put("<xmlattr>.wheelDiameter", axles.front_axle.wheel_diameter);
    front_axle_tree.put("<xmlattr>.trackWidth", axles.front_axle.track_width);
    front_axle_tree.put("<xmlattr>.positionX", axles.front_axle.position_x);
    front_axle_tree.put("<xmlattr>.positionZ", axles.front_axle.position_z);
    ptree & rear_axle_tree = vehicle_tree.add("Axles.RearAxle", "");
    rear_axle_tree.put("<xmlattr>.maxSteering", axles.rear_axle.max_steering);
    rear_axle_tree.put("<xmlattr>.wheelDiameter", axles.rear_axle.wheel_diameter);
    rear_axle_tree.put("<xmlattr>.trackWidth", axles.rear_axle.track_width);
    rear_axle_tree.put("<xmlattr>.positionX", axles.rear_axle.position_x);
    rear_axle_tree.put("<xmlattr>.positionZ", axles.rear_axle.position_z);
    std::stringstream ss;
    boost::property_tree::write_xml(ss, pt);
    return ss.str();
  }
};
}  // namespace entity
}  // namespace traffic_simulator

#endif  // TRAFFIC_SIMULATOR__ENTITY__VEHICLE_PARAMETER_HPP_
