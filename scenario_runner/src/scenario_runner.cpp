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

#include <scenario_runner/scenario_runner.hpp>
#include <rclcpp_components/register_node_macro.hpp>

#include <memory>
#include <string>

namespace scenario_runner
{
ScenarioRunner::ScenarioRunner(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("scenario_runner", options),
  service_client{create_client<GetScenario>("launcher_msg")},
  port{8080},
  scenario{
    ament_index_cpp::get_package_share_directory("scenario_runner") + "/test/success.xosc"}
{
  using std::chrono_literals::operator"" s;

  while (!(*service_client).wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(get_logger(), "Interrupted while waiting for service.");
      rclcpp::shutdown();
    } else {
      RCLCPP_INFO(get_logger(), "Waiting for service...");
    }
  }
}

ScenarioRunner::Result ScenarioRunner::on_configure(const rclcpp_lifecycle::State &)
{
  using scenario_runner::ScenarioRunner;

  auto request {std::make_shared<GetScenario::Request>()};

  (*request).scenario = "REQUEST!";

  auto result {
    (*service_client).async_send_request(request)
  };

  if (rclcpp::spin_until_future_complete(get_node_base_interface(), result) ==
    rclcpp::executor::FutureReturnCode::SUCCESS)
  {
    RCLCPP_INFO(get_logger(), "Received scenario path: '%s'", result.get()->launcher_msg.c_str());
  } else {
    RCLCPP_ERROR(get_logger(), "Problem while waiting for response.");
  }

  // DEBUG();
  // result.wait();
  // DEBUG();

  RCLCPP_INFO(get_logger(), "Received scenario path: '%s'", result.get()->launcher_msg.c_str());

  try {
    RCLCPP_INFO(get_logger(), "Loading scenario \"%s\"", scenario.c_str());
    evaluate.rebind<OpenScenario>(scenario, address, port);
  } catch (const scenario_runner::SyntaxError & error) {
    RCLCPP_ERROR(get_logger(), "\x1b[1;31m%s.\x1b[0m", error.what());
    return ScenarioRunner::Result::FAILURE;
  }

  RCLCPP_INFO(get_logger(), "Connecting simulator via %s:%d", address.c_str(), port);
  evaluate.as<OpenScenario>().init();

  return ScenarioRunner::Result::SUCCESS;
}

ScenarioRunner::Result ScenarioRunner::on_activate(const rclcpp_lifecycle::State &)
{
  using std::literals::chrono_literals::operator"" ms;

  timer = create_wall_timer(
    50ms,
    [&]()
    {
      try {
        if (!evaluate.as<OpenScenario>().complete()) {
          const auto result {evaluate.as<OpenScenario>()()};

          RCLCPP_INFO(get_logger(), "[Storyboard: %s]", boost::lexical_cast<std::string>(result));

          RCLCPP_INFO(
            get_logger(),
            "[%d standby (=> %d) => %d running (=> %d) => %d complete]",
            scenario_runner::standby_state.use_count() - 1,
            scenario_runner::start_transition.use_count() - 1,
            scenario_runner::running_state.use_count() - 1,
            scenario_runner::stop_transition.use_count() - 1,
            scenario_runner::complete_state.use_count() - 1);
        } else {
          deactivate();
        }
      } catch (const scenario_runner::Command & command) {
        switch (command) {
          case scenario_runner::Command::exitSuccess:
            RCLCPP_INFO(get_logger(), "\x1b[1;32mSimulation succeeded.\x1b[0m");
            deactivate();
            break;

          default:
          case scenario_runner::Command::exitFailure:
            RCLCPP_INFO(get_logger(), "\x1b[1;31mSimulation failed.\x1b[0m");
            deactivate();
        }
      } catch (const scenario_runner::ImplementationFault & error) {
        RCLCPP_ERROR(get_logger(), "%s.", error.what());
        deactivate();
      } catch (const std::exception & error) {
        RCLCPP_ERROR(get_logger(), "%s.", error.what());
        deactivate();
      }
    });

  return ScenarioRunner::Result::SUCCESS;
}

ScenarioRunner::Result ScenarioRunner::on_deactivate(const rclcpp_lifecycle::State &)
{
  timer.reset();
  return ScenarioRunner::Result::SUCCESS;
}

ScenarioRunner::Result ScenarioRunner::on_cleanup(const rclcpp_lifecycle::State &)
{
  evaluate = unspecified;
  return ScenarioRunner::Result::SUCCESS;
}

ScenarioRunner::Result ScenarioRunner::on_shutdown(const rclcpp_lifecycle::State &)
{
  return ScenarioRunner::Result::SUCCESS;
}

ScenarioRunner::Result ScenarioRunner::on_error(const rclcpp_lifecycle::State &)
{
  return ScenarioRunner::Result::SUCCESS;
}
}  // namespace scenario_runner

RCLCPP_COMPONENTS_REGISTER_NODE(scenario_runner::ScenarioRunner)
