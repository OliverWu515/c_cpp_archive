cmake_minimum_required(VERSION 3.0.2)
project(HW3)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

# find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

catkin_package()

include_directories(
  include
  SYSTEM
  ${catkin_INCLUDE_DIRS}
  ${Eigen3_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(hough src/hw3_hough.cpp)

target_link_libraries(hough
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
)
