cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(rosbag_to_udp)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  roscpp_serialization
  rostime
  cpp_common
  rosbag
  rosbag_storage
  message_generation
  velodyne_msgs
)

find_package(Boost REQUIRED COMPONENTS thread system)

include_directories(/opt/ros/kinetic/include/)
link_directories(/opt/ros/kinetic/lib/)

add_executable(vlp16_emu src/main.cc)
target_link_libraries(vlp16_emu rosbag rosbag_storage roscpp roscpp_serialization rostime cpp_common boost_system)
