# Adaptive Cruise Control (ACC) Simulation for ROS 2

A comprehensive ROS 2 simulation package demonstrating adaptive cruise control with two vehicles. The project follows **SOLID** and **RAII** principles for clean, reusable component design.

## Features

- **Two-vehicle simulation**: Lead vehicle and following vehicle with ACC control
- **Adaptive Cruise Control**: Rear vehicle automatically maintains desired distance to front vehicle while tracking desired speed
- **RViz Visualization**: Real-time 3D visualization of vehicles, road, and velocity vectors
- **SOLID Principles**: Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion
- **RAII Pattern**: Automatic resource management using smart pointers (`std::unique_ptr`)
- **Extensible Architecture**: Easy to add new control algorithms or dynamics models

## Architecture

### Core Components

#### 1. **Vehicle** (`vehicle.hpp`)
- **Responsibility**: Manages vehicle state and physics simulation
- **Design Pattern**: RAII - Uses smart pointers for dynamics model ownership
- **Encapsulation**: Position, velocity, acceleration, heading, and dimensions

#### 2. **IDynamicsModel Interface** (`vehicle.hpp`)
- **Responsibility**: Abstract interface for vehicle dynamics (Dependency Inversion)
- **Implementation**: `KinematicModel` - Simple kinematic equations with acceleration limits

#### 3. **Road** (`road.hpp`)
- **Responsibility**: Defines road properties and lane geometry
- **Single Responsibility**: Only handles road configuration, not vehicles or control

#### 4. **IACCController Interface** (`acc_controller.hpp`)
- **Responsibility**: Abstract interface for ACC control logic (Dependency Inversion)
- **Implementation**: `SimpleACCController` - Uses distance and speed error feedback

#### 5. **ACCSimulation** (`acc_controller.hpp`)
- **Responsibility**: Orchestrates simulation lifecycle and vehicle management
- **Design Pattern**: RAII - Manages vehicle and controller lifetimes
- **Open/Closed Principle**: Open for extension (new controllers), closed for modification

#### 6. **ACCSimulationNode** (`acc_simulation_node.hpp`)
- **Responsibility**: ROS 2 integration and RViz marker publishing
- **Lifecycle**: Timer-based simulation updates with configurable frequency

## SOLID Principles Implementation

### Single Responsibility Principle (SRP)
- `Vehicle`: Only manages vehicle state and position updates
- `IDynamicsModel`: Only handles physics simulation
- `Road`: Only defines road properties
- `IACCController`: Only calculates control commands

### Open/Closed Principle (OCP)
- New dynamics models can be added by implementing `IDynamicsModel` without modifying existing code
- New control algorithms can be added by implementing `IACCController` without modifying existing code

### Liskov Substitution Principle (LSP)
- `KinematicModel` can be substituted for any `IDynamicsModel`
- `SimpleACCController` can be substituted for any `IACCController`

### Interface Segregation Principle (ISP)
- `IDynamicsModel` interface is minimal and focused
- `IACCController` interface is minimal and focused
- Clients depend only on what they use

### Dependency Inversion Principle (DIP)
- `Vehicle` depends on `IDynamicsModel` abstraction, not concrete implementations
- `ACCSimulation` depends on `IACCController` abstraction
- High-level modules don't depend on low-level modules; both depend on abstractions

## RAII Implementation

All resource management uses RAII with smart pointers:

```cpp
// Vehicle owns dynamics model
std::unique_ptr<IDynamicsModel> dynamics_;

// ACCSimulation owns vehicles
std::vector<std::unique_ptr<Vehicle>> vehicles_;

// ACCSimulation owns controller
std::unique_ptr<IACCController> controller_;
```

No manual cleanup needed - resources are automatically freed when objects are destroyed.

## Building

```bash
cd ~/ros2_ws
source /opt/ros/kilted/setup.bash
colcon build --packages-select acc_simulation
source install/setup.bash
```

## Running the Simulation

### Basic launch (uses defaults):
```bash
ros2 launch acc_simulation acc_simulation.launch.py
```

### With custom parameters:
```bash
ros2 launch acc_simulation acc_simulation.launch.py \
  desired_speed:=25.0 \
  desired_distance:=40.0 \
  kp:=0.6 \
  kv:=0.4
```

### Parameters:
- **desired_speed**: Target speed for ACC controller in m/s (default: 20.0)
- **desired_distance**: Desired distance to lead vehicle in meters (default: 30.0)
- **kp**: Proportional gain for distance control (default: 0.5)
- **kv**: Proportional gain for speed control (default: 0.3)
- **update_frequency**: Simulation update frequency in Hz (default: 20.0)

## RViz Visualization

The simulation publishes to `/visualization_marker_array` with:
- **Gray road surface**: Road representation
- **White lane markings**: Lane center markings
- **Green vehicle**: Lead vehicle (front)
- **Red vehicle**: Following vehicle (ego) with ACC control
- **Blue arrows**: Velocity vectors for each vehicle

## Control Algorithm

The ACC controller uses a combined distance and speed error feedback:

$$a_{target} = k_p \cdot e_{distance} + k_v \cdot e_{velocity}$$

Where:
- $e_{distance}$ = (desired_distance - actual_distance)
- $e_{velocity}$ = (desired_speed - actual_velocity)
- $k_p$ = proportional gain for distance control
- $k_v$ = proportional gain for velocity control

The calculated acceleration is clamped to realistic limits:
- Maximum acceleration: 5.0 m/s²
- Maximum deceleration: 8.0 m/s²

## Extending the Project

### Adding a New Dynamics Model

```cpp
class AdvancedDynamicsModel : public IDynamicsModel {
public:
    void update(VehicleState& state, double dt) override {
        // Your physics implementation
    }
};

// Use it:
auto vehicle = std::make_unique<Vehicle>(
    0, 100.0, 0.0, 4.5, 2.0,
    std::make_unique<AdvancedDynamicsModel>()
);
```

### Adding a New ACC Controller

```cpp
class PIDACCController : public IACCController {
public:
    double calculateAcceleration(const Vehicle& ego_vehicle,
                                const Vehicle& lead_vehicle) override {
        // Your control logic with PID
    }
};

// Use it:
auto controller = std::make_unique<PIDACCController>();
simulation_.setController(std::move(controller));
```

### Adding More Vehicles

```cpp
// Add lead vehicle
simulation_.addVehicle(std::make_unique<Vehicle>(0, 150.0, 0.0));

// Add second vehicle
simulation_.addVehicle(std::make_unique<Vehicle>(1, 100.0, 0.0));

// Add third vehicle
simulation_.addVehicle(std::make_unique<Vehicle>(2, 50.0, 0.0));
```

## File Structure

```
acc_simulation/
├── include/acc_simulation/
│   ├── vehicle.hpp           # Vehicle and dynamics model classes
│   ├── acc_controller.hpp    # ACC controller and simulation classes
│   └── road.hpp              # Road configuration class
├── src/
│   ├── main.cpp              # Entry point
│   ├── vehicle.cpp           # Vehicle implementation
│   ├── acc_controller.cpp    # ACC and simulation implementation
│   └── acc_simulation_node.cpp    # ROS 2 node implementation
├── launch/
│   ├── acc_simulation.launch.py    # Launch file with parameters
│   └── acc_simulation.rviz         # RViz configuration
├── msg/
│   ├── VehicleState.msg      # Vehicle state message (optional)
│   ├── RoadConfig.msg        # Road config message (optional)
│   └── ACCCommand.msg        # ACC command message (optional)
├── CMakeLists.txt
└── package.xml
```

## Performance

- **Time step**: 50ms (20 Hz update rate)
- **Simulation time**: Real-time
- **CPU usage**: Minimal (< 1% on typical hardware)
- **Memory**: ~10 MB for base simulation with 2 vehicles

## Testing

To test the ACC controller behavior:

1. Launch the simulation
2. Observe the red vehicle (ego) accelerating from 10 m/s to match the green vehicle
3. The distance between vehicles should stabilize at ~30 meters
4. Adjust `kp` and `kv` to tune controller responsiveness

## Future Enhancements

- [ ] Multi-vehicle platoon with multiple ACC instances
- [ ] Scenario playback (lead vehicle speed profile)
- [ ] Performance metrics collection
- [ ] Unit tests for control algorithms
- [ ] Different lane behaviors
- [ ] Emergency braking scenarios
- [ ] Traffic scenarios with multiple vehicles changing lanes

## License

Apache License 2.0

## Author

Adrian Carauleanu
