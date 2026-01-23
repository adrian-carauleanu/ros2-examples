#!/bin/bash
# Quick setup and run script for ACC Simulation

set -e

echo "=== ACC Simulation Setup and Run ==="
echo

# Source ROS environment
echo "Sourcing ROS 2 Kilted environment..."
source /opt/ros/kilted/setup.bash

# Navigate to workspace
cd /home/adrian-carauleanu/ros2_ws

echo "Building ACC simulation package..."
colcon build --packages-select acc_simulation

echo "Sourcing workspace..."
source install/setup.bash

echo
echo "=== Setup Complete ==="
echo
echo "To run the simulation with default parameters:"
echo "  ros2 launch acc_simulation acc_simulation.launch.py"
echo
echo "To run with custom parameters:"
echo "  ros2 launch acc_simulation acc_simulation.launch.py desired_speed:=25.0 desired_distance:=40.0"
echo
echo "Available parameters:"
echo "  - desired_speed: Target speed in m/s (default: 20.0)"
echo "  - desired_distance: Desired distance to lead vehicle in meters (default: 30.0)"
echo "  - kp: Proportional gain for distance (default: 0.5)"
echo "  - kv: Proportional gain for velocity (default: 0.3)"
echo "  - update_frequency: Simulation frequency in Hz (default: 20.0)"
echo
