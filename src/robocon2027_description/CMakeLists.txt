cmake_minimum_required(VERSION 3.8)
project(robocon2027_description)

find_package(ament_cmake REQUIRED)

install(
  DIRECTORY
    meshes
    worlds
    launch
    urdf
    config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
