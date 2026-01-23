# Vehicle Management Guide

This guide explains how to dynamically add and remove vehicles during the ACC simulation.

## Overview

The ACC simulation now supports dynamic vehicle management through ROS 2 services. You can add new vehicles or remove existing ones without stopping the simulation.

## Service Interfaces

### Add Vehicle Service
- **Service Name**: `/acc_simulation_node/add_vehicle`
- **Service Type**: `std_srvs/srv/Empty`
- **Behavior**: Creates a new vehicle positioned 40 meters behind the last vehicle
- **Vehicle Configuration**:
  - Length: 4.5 meters
  - Width: 2.0 meters
  - Initial Velocity: 10 m/s
  - Heading: π/2 radians (vertical orientation)
  - Lane: Center lane (default lane 0)

### Remove Vehicle Service
- **Service Name**: `/acc_simulation_node/remove_vehicle`
- **Service Type**: `std_srvs/srv/Empty`
- **Behavior**: Removes the last vehicle added to the simulation
- **Constraints**: At least one vehicle must remain in the simulation

## Usage Methods

### Method 1: Command Line (ros2 service call)

**Add a vehicle:**
```bash
ros2 service call /acc_simulation_node/add_vehicle std_srvs/srv/Empty {}
```

**Remove a vehicle:**
```bash
ros2 service call /acc_simulation_node/remove_vehicle std_srvs/srv/Empty {}
```

### Method 2: RViz Panel

The RViz panel plugin (`acc_simulation_panel`) provides a graphical interface for vehicle management.

**To add the panel to RViz:**
1. In RViz, go to `Panels` → `Add new panel`
2. Select `acc_simulation_panel::VehicleControlPanel`
3. The panel will appear in the RViz interface

**Panel Features:**
- **Add Vehicle Button**: Dynamically adds a new vehicle to the simulation
- **Remove Vehicle Button**: Removes the last vehicle from the simulation
- **Update Info Button**: Refreshes status information
- **Status Display**: Shows current connection state and simulation status

### Method 3: Programmatic (C++)

If you're building your own ROS 2 application, you can call the services programmatically:

```cpp
#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/empty.hpp>

// Create a client
auto client = node->create_client<std_srvs::srv::Empty>(
    "/acc_simulation_node/add_vehicle");

// Wait for service to be available
while (!client->wait_for_service(std::chrono::seconds(1))) {
    // Service not available
}

// Call the service
auto request = std::make_shared<std_srvs::srv::Empty::Request>();
auto future = client->async_send_request(request);
auto result = future.get();
```

## Behavior and Constraints

### Vehicle Addition
- New vehicles are positioned 40 meters behind the last vehicle in the Y-axis
- New vehicles have a sequential ID (0, 1, 2, ...)
- New vehicles are initialized with:
  - Velocity: 10 m/s
  - Acceleration: 0 m/s²
  - Heading: π/2 radians (vertical)

### Vehicle Removal
- Only the most recently added vehicle is removed
- At least one vehicle must remain in the simulation
- After removal, vehicle count decreases by 1

### Visualization
In RViz, vehicles are represented as colored cubes:
- **Green cube**: Lead vehicle (vehicle 0)
- **Red cubes**: Following vehicles
- **Blue arrows**: Velocity vectors indicating direction of motion

## Launch the Simulation with Panel Support

```bash
cd ~/ros2_ws
source install/setup.bash
ros2 launch acc_simulation acc_simulation.launch.py
```

Then in a separate terminal:
```bash
rviz2
```

Add the Vehicle Control Panel from the Panels menu to manage vehicles interactively.

## Troubleshooting

### "Service not available" error
- Ensure the simulation node is running
- Check that the service name is correct: `/acc_simulation_node/add_vehicle` or `/acc_simulation_node/remove_vehicle`
- Use `ros2 service list` to verify available services

### Cannot remove vehicle
- This is expected behavior when only one vehicle remains
- The simulation requires at least one vehicle to function

### Panel doesn't appear in RViz
- Ensure `acc_simulation_panel` package was built successfully: `colcon build --packages-select acc_simulation_panel`
- Verify RViz is using the updated ROS 2 setup: `source install/setup.bash` before launching RViz

## Advanced: Adding Vehicle Count Service

To query the current number of vehicles, you can add a custom service. This would require:
1. Creating a custom service message (e.g., `acc_simulation/srv/GetVehicleCount.srv`)
2. Adding a service handler in `ACCSimulationNode`
3. Updating the RViz panel to call this service

See [EXTENSIONS.md](EXTENSIONS.md) for more details on extending the system.
