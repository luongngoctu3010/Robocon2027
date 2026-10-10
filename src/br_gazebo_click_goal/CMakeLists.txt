cmake_minimum_required(VERSION 3.16)

project(br_gazebo_click_goal)

find_package(ament_cmake REQUIRED)
find_package(gz-gui REQUIRED)
find_package(gz-transport REQUIRED)
find_package(gz-msgs REQUIRED)

add_library(BrClickGoalPlugin SHARED
  src/br_click_goal_plugin.cpp
)

target_link_libraries(BrClickGoalPlugin
  gz-gui::gz-gui
  gz-transport::gz-transport
  gz-msgs::gz-msgs
)

install(
  TARGETS BrClickGoalPlugin
  LIBRARY DESTINATION lib
)

install(
  FILES package.xml
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
