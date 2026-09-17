cmake_minimum_required(VERSION 3.8)
project(robocon2027_description)

find_package(ament_cmake REQUIRED)
find_package(xacro REQUIRED)
find_package(robot_state_publisher REQUIRED)

install(
  DIRECTORY
    meshes
    urdf
    worlds
    launch
    config
  DESTINATION share/${PROJECT_NAME}
)

install(
  PROGRAMS
    scripts/mecanum_controller.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME mecanum_controller
)

ament_package()
