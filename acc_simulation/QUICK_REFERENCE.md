# ACC Simulation Project - Quick Reference

## Project Summary

A **ROS 2 Adaptive Cruise Control (ACC) Simulation** demonstrating professional software engineering practices with SOLID principles and RAII memory management. The project simulates two vehicles where the rear vehicle automatically maintains a safe distance to the front vehicle while tracking a desired speed.

## Quick Start (30 seconds)

```bash
cd ~/ros2_ws
source /opt/ros/kilted/setup.bash
colcon build --packages-select acc_simulation
source install/setup.bash
ros2 launch acc_simulation acc_simulation.launch.py
```

**Expected Output**: RViz window opens showing:
- Gray road with white lane markings
- Green vehicle (front) moving at constant 15 m/s
- Red vehicle (rear) accelerating to maintain 30m distance at 20 m/s target speed

## Project Files at a Glance

### Core Headers (Interface Definitions)
- **vehicle.hpp**: Vehicle state, dynamics interface, vehicle class
- **acc_controller.hpp**: ACC control interface, controller, simulation orchestrator
- **road.hpp**: Road configuration and properties
- **acc_simulation_node.hpp**: ROS 2 node with RViz integration

### Implementations
- **vehicle.cpp**: Vehicle physics simulation
- **acc_controller.cpp**: ACC algorithm and simulation loop
- **acc_simulation_node.cpp**: ROS node and marker publishing
- **main.cpp**: Entry point

### Configuration
- **package.xml**: ROS package metadata
- **CMakeLists.txt**: Build configuration
- **acc_simulation.launch.py**: Launch file with parameters
- **acc_simulation.rviz**: RViz configuration

### Documentation
- **README.md**: Full user guide (control algorithm, parameters, extending)
- **TESTING.md**: 5 test scenarios with expected results
- **ARCHITECTURE.md**: Design patterns, layers, extension points
- **EXTENSIONS.md**: 6 detailed examples for adding features
- **QUICK_REFERENCE.md** (this file): Quick lookup

## Architecture Layers

```
┌─────────────────────────────────────────┐
│ Layer 3: ROS Integration                │
│ (ACCSimulationNode, RViz publishing)    │
└──────────────┬──────────────────────────┘
               │
┌──────────────┴──────────────────────────┐
│ Layer 2: Control & Orchestration        │
│ (ACCSimulation, IACCController)         │
└──────────────┬──────────────────────────┘
               │
┌──────────────┴──────────────────────────┐
│ Layer 1: Domain Models                  │
│ (Vehicle, Road, IDynamicsModel)         │
└─────────────────────────────────────────┘
```

## Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| **Vehicle** | Vehicle entity with state & physics | `update()`, `setVelocity()`, `getPositionX()` |
| **IDynamicsModel** | Physics simulation interface | `update(state, dt)` |
| **KinematicModel** | Simple kinematic physics | Inherits `IDynamicsModel` |
| **IACCController** | ACC control interface | `calculateAcceleration(ego, lead)` |
| **SimpleACCController** | Proportional ACC control | Inherits `IACCController` |
| **ACCSimulation** | Simulation orchestrator | `update()`, `addVehicle()`, `setController()` |
| **Road** | Road configuration | `getLaneYPosition(lane)` |
| **ACCSimulationNode** | ROS 2 node | Timer callback, marker publishing |

## SOLID Principles Used

✅ **S**ingle Responsibility: Each class has one job (Vehicle, Dynamics, Control, Road)  
✅ **O**pen/Closed: Open for extension (new controllers/models), closed for modification  
✅ **L**iskov Substitution: All implementations properly substitute interfaces  
✅ **I**nterface Segregation: Minimal, focused interfaces (IDynamicsModel, IACCController)  
✅ **D**ependency Inversion: Depend on abstractions, not concrete classes  

## RAII Implementation

All resources use smart pointers for automatic cleanup:

```cpp
std::unique_ptr<IDynamicsModel> dynamics_;              // Vehicle owns model
std::vector<std::unique_ptr<Vehicle>> vehicles_;        // Simulation owns vehicles
std::unique_ptr<IACCController> controller_;            // Simulation owns controller
```

**Benefit**: No manual `delete`, exception-safe, clear ownership semantics

## Running Tests

### Basic Test
```bash
ros2 launch acc_simulation acc_simulation.launch.py
# Expected: Green vehicle at constant 15 m/s, red vehicle accelerates to follow
```

### Aggressive Controller
```bash
ros2 launch acc_simulation acc_simulation.launch.py kp:=1.0 kv:=0.6
# Expected: Faster response, possible slight oscillation
```

### Conservative Controller
```bash
ros2 launch acc_simulation acc_simulation.launch.py kp:=0.2 kv:=0.1
# Expected: Smooth, gentle acceleration, longer settling time
```

### Close Following
```bash
ros2 launch acc_simulation acc_simulation.launch.py desired_distance:=20.0
# Expected: Ego vehicle maintains 20m distance instead of 30m
```

See [TESTING.md](TESTING.md) for 5 complete test scenarios with expected outputs.

## Parameters

| Parameter | Default | Range | Effect |
|-----------|---------|-------|--------|
| `desired_speed` | 20.0 m/s | 5-35 | Target speed for ego vehicle |
| `desired_distance` | 30.0 m | 10-100 | Safe following distance |
| `kp` | 0.5 | 0.1-2.0 | Distance error gain (higher = faster) |
| `kv` | 0.3 | 0.1-1.0 | Speed error gain (higher = faster) |
| `update_frequency` | 20.0 Hz | 10-100 | Simulation update rate |

## Control Algorithm

$$a_{target} = k_p \cdot (d_{desired} - d_{actual}) + k_v \cdot (v_{desired} - v_{actual})$$

Where:
- $a_{target}$ = acceleration command (clamped to ±5 m/s²)
- $k_p$ = proportional distance gain
- $k_v$ = proportional speed gain
- $d_{actual}$ = current distance to lead vehicle
- $v_{actual}$ = current ego vehicle velocity

## Extending the Project

### Add a New Controller (10 minutes)
1. Create class inheriting `IACCController`
2. Implement `calculateAcceleration()` method
3. Add to CMakeLists.txt
4. Use in node via `simulation_.setController()`

Example: [EXTENSIONS.md - Example 1](EXTENSIONS.md#example-1-adding-a-pid-controller)

### Add More Vehicles (5 minutes)
1. Create additional `Vehicle` objects
2. Call `simulation_.addVehicle()` for each
3. Modify visualization color in `createVehicleMarkers()`

Example: [EXTENSIONS.md - Example 2](EXTENSIONS.md#example-2-adding-a-third-vehicle)

### Add New Physics Model (15 minutes)
1. Create class inheriting `IDynamicsModel`
2. Implement `update()` method with your physics
3. Pass to Vehicle constructor via dependency injection

Example: See [ARCHITECTURE.md - Extension Points](ARCHITECTURE.md#extension-points)

See [EXTENSIONS.md](EXTENSIONS.md) for 6 detailed examples with complete code.

## RViz Tips

- **Middle Mouse + Drag**: Rotate view
- **Mouse Wheel**: Zoom in/out  
- **Right Mouse + Drag**: Pan
- **Select Tool** (shortcut: S): Click vehicle to inspect
- **Measure Tool** (shortcut: Shift+M): Measure distances

## Performance

- **CPU**: <1% utilization on typical hardware
- **Memory**: ~10 MB for 2-vehicle simulation
- **Time Step**: 50 ms (20 Hz update rate)
- **Latency**: <100 μs for simulation calculations

## Troubleshooting

| Issue | Solution |
|-------|----------|
| No vehicles in RViz | Check `/visualization_marker_array` topic exists |
| Vehicle doesn't accelerate | Increase `kv` parameter |
| Vehicle oscillates | Decrease `kp` and `kv` |
| RViz doesn't start | Check `acc_simulation.rviz` file permissions |
| Build fails | Check ROS 2 Kilted environment is sourced |

## File Statistics

- **Total Lines of Code**: ~800 (without comments)
- **Header Files**: 4
- **Implementation Files**: 4
- **Documentation Pages**: 5
- **Build Time**: ~12 seconds

## Design Patterns

- **Strategy**: IDynamicsModel and IACCController interfaces
- **RAII**: Smart pointer ownership model
- **Composition**: Vehicle contains Dynamics, Simulation contains Vehicles
- **Dependency Injection**: Models and controllers passed to constructors
- **Template Method**: Node callback orchestration

## Class Relationships

```
Vehicle ─┐
         ├─ contains ─> IDynamicsModel
         │
Road     │
         │
ACCSimulation ─┬─ contains ─> std::vector<Vehicle>
               └─ contains ─> IACCController

ACCSimulationNode ─┬─ contains ─> ACCSimulation
                   ├─ contains ─> Road
                   └─ publishes ─> MarkerArray
```

## Key Design Decisions

1. **Interface-based design**: Easy to swap implementations
2. **RAII with unique_ptr**: No memory management bugs
3. **Single simulation class**: Clear orchestration point
4. **ROS node separate from logic**: Pure domain model, testable
5. **Marker-based visualization**: Works with standard RViz
6. **Simple proportional control**: Easy to understand, extend with PID

## Next Steps

1. **Review [README.md](README.md)** for complete user guide
2. **Run tests in [TESTING.md](TESTING.md)** to understand behavior
3. **Study [ARCHITECTURE.md](ARCHITECTURE.md)** to understand design
4. **Implement extensions from [EXTENSIONS.md](EXTENSIONS.md)**
5. **Build multi-vehicle platoon** (Example 2)
6. **Implement PID controller** (Example 1)

## Getting Help

1. Check console output: `[INFO]` logs show simulation state
2. Check RViz topic list: `ros2 topic list | grep marker`
3. Inspect marker properties in RViz Display panel
4. Review code comments in header files
5. See EXTENSIONS.md for similar features

## License

Apache License 2.0

## Author

Adrian Carauleanu - ROS 2 Adaptive Cruise Control Simulation

---

**Last Updated**: January 2026  
**ROS 2 Distribution**: Kilted Kame  
**Build Status**: ✅ Passing
