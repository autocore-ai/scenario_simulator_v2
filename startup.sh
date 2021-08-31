#!/bin/bash

set -o xtrace
set -e

function donextstep() {
    echo "do next step!"
}

ros2 launch scenario_test_runner scenario_test_runner.launch.py workflow:='/home/ubuntu/Desktop/scenario_simulator_ws/work_flow/work_flow.yaml' global_frame_rate:=20 vehicle_model:=lexus sensor_model:=aip_xx1 || donextstep
# ls /tmp/scenario_test_runner || donextstep
# cat /tmp/scenario_test_runner/result.junit.xml
ros2 run scenario_test_runner result_checker /tmp/scenario_test_runner/result.junit.xml
