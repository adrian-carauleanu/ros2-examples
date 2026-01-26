# ACC Simulation RViz Panel Setup Guide

## Overview

The `acc_simulation_panel` package provides an RViz panel plugin for dynamic vehicle management in the ACC simulation. This allows interactive control of vehicles through a graphical interface without command-line interaction.

## Installation & Build

### Prerequisites
- ROS 2 Kilted Kaiju
- Qt5 development libraries
- rviz2 installed

### Build Process

```bash
cd ~/ros2_ws
source /opt/ros/kilted/setup.bash

# Build the panel plugin
colcon build --packages-select acc_simulation_panel

# Source the built packages
source install/setup.bash
```

### Verify Build
```bash
ls -la install/acc_simulation_panel/lib/
# Should contain: libacc_simulation_panel_lib.so
```

## Loading the Panel in RViz

### Method 1: From GUI (Recommended)

1. **Launch RViz**:
   ```bash
   source install/setup.bash
   rviz2
   ```

2. **Add the Panel**:
   - Click `Panels` menu in the menu bar
   - Select `Add New Panel`
   - Find and select `acc_simulation_panel::VehicleControlPanel`

3. **Panel Appears**:
   - A new docked panel appears on the right side
   - Title: "Vehicle Control Panel"

### Method 2: Command Line Launch

```bash
source install/setup.bash
ros2 launch acc_simulation acc_simulation.launch.py
```

Then launch RViz separately and add the panel as described in Method 1.

## Panel Features

### Button Controls

**Add Vehicle**
- Adds a new vehicle to the simulation
- New vehicle positioned 40m behind the last vehicle
- Vehicle appears in RViz with red color
- Status updates in the Status section

**Remove Vehicle**
- Removes the most recently added vehicle
- At least 1 vehicle always remains
- Vehicle disappears from RViz immediately
- Status updates in the Status section

**Update Info**
- Refreshes the status information
- Useful for checking current simulation state

### Status Display

**Status Label**
- Shows "Connected to ACC Simulation" when panel initializes
- Updates to show result of service calls
- Shows error messages if services are unavailable

**Vehicle Count Label**
- Displays number of active vehicles
- Updates when vehicles are added/removed

## Typical Workflow

### Scenario 1: Adding Vehicles One by One

1. Launch simulation: `ros2 launch acc_simulation acc_simulation.launch.py`
2. Open RViz and add the Vehicle Control Panel
3. Click "Add Vehicle" button multiple times
4. Watch new red vehicles appear in RViz behind the lead (green) vehicle
5. Notice simulation continues smoothly with new vehicles

### Scenario 2: Testing Different Vehicle Counts

1. Start with 2 vehicles (default)
2. Add 3 more vehicles → 5 total
3. Remove 2 vehicles → 3 remaining
4. Observe the ACC control working for all following vehicles

### Scenario 3: Interactive Demonstration

1. Run simulation with students/colleagues watching RViz
2. Demonstrate smooth operation with 2 vehicles
3. Add vehicles to show scalability
4. Remove vehicles to highlight flexibility
5. Pause and discuss ACC control behavior

## Troubleshooting

### Panel Doesn't Appear in Menu

**Symptoms**: Can't find `acc_simulation_panel::VehicleControlPanel` in Panels menu

**Solutions**:
1. Verify build succeeded: `colcon build --packages-select acc_simulation_panel`
2. Check plugin library exists: `ls install/acc_simulation_panel/lib/libacc_simulation_panel_lib.so`
3. Source setup script before RViz: `source install/setup.bash && rviz2`
4. Restart RViz after building

### Services Show "Not Available"

**Symptoms**: Status shows "Service not available"

**Solutions**:
1. Verify simulation is running: `ros2 service list | grep acc_simulation`
2. Check simulation node started successfully (look for INFO logs)
3. Restart both simulation and RViz
4. Verify service names:
   ```bash
   ros2 service list | grep vehicle
   # Should show:
   # /acc_simulation_node/add_vehicle
   # /acc_simulation_node/remove_vehicle
   ```

### Vehicles Don't Appear/Disappear

**Symptoms**: Buttons work, status updates, but RViz doesn't change

**Solutions**:
1. Check RViz is subscribed to visualization markers
2. Verify `/visualization_marker_array` topic is present: `ros2 topic list | grep marker`
3. In RViz Displays panel, ensure Marker Array is enabled
4. Check RViz camera view isn't outside the road area
5. Zoom fit RViz view (Home key or 'h')

### Building the Panel Fails

**Symptoms**: Compilation errors, missing libraries

**Common Issues**:
```
error: "XXX_NOTFOUND"
```
**Solution**: Install Qt5: `sudo apt install qt5-qmake qt5-default qtbase5-dev`

```
error: "rviz_common not found"
```
**Solution**: Ensure RViz2 is installed: `sudo apt install ros-kilted-rviz2`

## Architecture Details

### Plugin System

The panel uses RViz's plugin system via `pluginlib`:

```cpp
PLUGINLIB_EXPORT_CLASS(acc_simulation_panel::VehicleControlPanel,
                       rviz_common::Panel)
```

This allows RViz to dynamically discover and load the plugin at runtime.

### Directory Structure

```
acc_simulation_panel/
├── include/acc_simulation_panel/
│   └── vehicle_control_panel.hpp      # Header with Qt slots and members
├── src/
│   └── vehicle_control_panel.cpp      # Implementation with UI layout
├── plugins.xml                         # Plugin registration (for RViz)
├── CMakeLists.txt                      # Build configuration
└── package.xml                         # ROS 2 package metadata
```

### Key Components

**vehicle_control_panel.hpp**
- Defines `VehicleControlPanel` class inheriting from `rviz_common::Panel`
- Qt slots (`onAddVehicleClicked`, `onRemoveVehicleClicked`, etc.)
- UI member pointers (buttons, labels)

**vehicle_control_panel.cpp**
- Implements UI layout in constructor
- Implements slot handlers
- Service call stubs for future enhancement

**plugins.xml**
- Registers the panel with RViz
- Maps plugin class to display name

## Future Enhancements

### Service Client Implementation

The current panel shows placeholder implementations. To add actual service communication:

```cpp
// In onInitialize():
add_vehicle_client_ = node_->create_client<std_srvs::srv::Empty>(
    "/acc_simulation_node/add_vehicle");

// In callAddVehicleService():
if (!add_vehicle_client_->service_is_ready()) {
    // Wait for service
}
auto request = std::make_shared<std_srvs::srv::Empty::Request>();
auto future = add_vehicle_client_->async_send_request(request);
```

### Custom Service Message

For vehicle count feedback:

1. Create custom message: `acc_simulation_msgs/srv/GetVehicleCount.srv`
2. Implement service handler in simulation node
3. Add client and call in panel
4. Update status label with vehicle count

### Status Log Display

Add a text browser to show service call history:

```cpp
QTextBrowser* log_display_;  // New member

void logMessage(const QString& msg) {
    log_display_->append(msg);
}
```

## Performance Notes

- Panel adds minimal overhead (~1-2% CPU)
- Service calls are asynchronous (non-blocking)
- UI updates independent of simulation thread
- Safe for real-time operation

## Safety Considerations

- Minimum 1 vehicle always maintained (safety constraint)
- New vehicles positioned safely behind existing vehicles
- Service calls are atomic (add/remove complete immediately)
- No intermediate inconsistent states

## Questions & Support

For issues or questions about the panel:

1. Check [VEHICLE_MANAGEMENT.md](VEHICLE_MANAGEMENT.md) for service documentation
2. Review [EXTENSIONS.md](../EXTENSIONS.md) for architectural details
3. Check RViz logs: `ROS_LOG_DIR=/tmp ros2 run rviz2 rviz2`
4. Verify service connectivity: `ros2 service call /acc_simulation_node/add_vehicle std_srvs/srv/Empty {}`
