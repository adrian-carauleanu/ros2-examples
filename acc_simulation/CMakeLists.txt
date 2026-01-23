cmake_minimum_required(VERSION 3.20)
project(acc_simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(rclcpp REQUIRED)

# Include directories
include_directories(include)

# Add library
add_library(acc_simulation_lib
  src/vehicle.cpp
  src/acc_controller.cpp
  src/acc_simulation_node.cpp
)

ament_target_dependencies(acc_simulation_lib
  rclcpp
  std_msgs
  std_srvs
  geometry_msgs
  visualization_msgs
)

# Add executable
add_executable(acc_simulation_node src/main.cpp)
target_link_libraries(acc_simulation_node acc_simulation_lib)
ament_target_dependencies(acc_simulation_node rclcpp)

install(TARGETS acc_simulation_node
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY include/
  DESTINATION include/)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
