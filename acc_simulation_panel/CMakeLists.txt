cmake_minimum_required(VERSION 3.20)
project(acc_simulation_panel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_ros REQUIRED)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rviz_common REQUIRED)

qt5_wrap_cpp(MOC_FILES
  include/acc_simulation_panel/vehicle_control_panel.hpp
)

add_library(acc_simulation_panel_lib SHARED
  src/vehicle_control_panel.cpp
  ${MOC_FILES}
)

target_include_directories(acc_simulation_panel_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(acc_simulation_panel_lib PUBLIC
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  rviz_common::rviz_common
  pluginlib::pluginlib
)

install(TARGETS acc_simulation_panel_lib
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

install(FILES plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

pluginlib_export_plugin_description_file(rviz_common plugins.xml)

ament_package()
