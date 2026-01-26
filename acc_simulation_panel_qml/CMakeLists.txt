cmake_minimum_required(VERSION 3.20)
project(acc_simulation_panel_qml)

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
find_package(Qt5 REQUIRED COMPONENTS Quick QuickWidgets Qml)
find_package(pluginlib REQUIRED)
find_package(rviz_common REQUIRED)
find_package(acc_simulation_interfaces REQUIRED)
find_package(ament_index_cpp REQUIRED)

qt5_wrap_cpp(MOC_FILES
  include/acc_simulation_panel/acc_simulation_panel.hpp
)

add_library(acc_simulation_panel_lib SHARED
  src/acc_simulation_panel.cpp
  ${MOC_FILES}
)

target_include_directories(acc_simulation_panel_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(acc_simulation_panel_lib PUBLIC
  Qt5::Quick 
  Qt5::QuickWidgets 
  Qt5::Qml
  rviz_common::rviz_common
  pluginlib::pluginlib
  ${acc_simulation_interfaces_TARGETS}
  ament_index_cpp::ament_index_cpp
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
