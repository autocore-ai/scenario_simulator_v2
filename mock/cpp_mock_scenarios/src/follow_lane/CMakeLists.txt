ament_auto_add_executable(acquire_position_in_world_frame
  acquire_position_in_world_frame.cpp
)
target_link_libraries(acquire_position_in_world_frame cpp_scenario_node)

install(TARGETS
  acquire_position_in_world_frame
  DESTINATION lib/cpp_mock_scenarios
)
