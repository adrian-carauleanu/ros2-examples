# ACC Simulation Architecture Document

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    ROS 2 Node Layer                         │
│          (ACCSimulationNode - ros2/rclcpp)                 │
│                                                             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │ Simulation Update Timer (20 Hz)                       │ │
│  │  └─> publish /visualization_marker_array to RViz    │ │
│  └───────────────────────────────────────────────────────┘ │
└──────────────────┬──────────────────────────────────────────┘
                   │
        ┌──────────┴──────────┐
        │ Orchestration Layer │
        │   (ACCSimulation)   │
        └──────────┬──────────┘
                   │
    ┌──────────────┼──────────────┬────────────────┐
    │              │              │                │
    v              v              v                v
┌─────────┐  ┌──────────┐  ┌──────────────┐  ┌────────┐
│ Vehicle │  │ Vehicle  │  │ IACCController
│ (Lead)  │  │ (Ego)    │  │ (Abstract)    │  │ Road   │
│         │  │          │  │               │  │        │
│ Dynamics│  │ Dynamics │  │ SimpleACC...  │  │ Lanes  │
│ Model   │  │ Model    │  │ (Concrete)    │  │        │
└─────────┘  └──────────┘  └──────────────┘  └────────┘
    │            │               │
    │            │               │
    │            │               │
    └────────────┴───────────────┘
            Control Loop
```

## Layered Architecture

### Layer 1: Core Domain Model (vehicle.hpp, road.hpp)
**Responsibility**: Pure domain logic, no ROS dependencies

```
VehicleState (Data)
    ↓
Vehicle (Entity)
    ├─ VehicleState
    └─ IDynamicsModel
        └─ KinematicModel

Road (Value Object)
    └─ Lane information
```

**Dependencies**: Only C++ STL (`<cmath>`, `<memory>`, `<vector>`)

### Layer 2: Control Logic (acc_controller.hpp)
**Responsibility**: Simulation orchestration and control algorithms

```
IACCController (Abstract)
    └─ SimpleACCController (Concrete)

ACCSimulation (Orchestrator)
    ├─ std::vector<Vehicle>
    ├─ IACCController
    └─ Simulation state
```

**Dependencies**: Core domain model layer

### Layer 3: ROS Integration (acc_simulation_node.hpp)
**Responsibility**: ROS 2 integration and visualization

```
ACCSimulationNode (ROS Node)
    ├─ ACCSimulation (composition)
    ├─ Road (composition)
    ├─ Publisher<MarkerArray>
    ├─ Timer
    └─ Marker generation
```

**Dependencies**: Layers 1 & 2 + rclcpp + RViz messages

## Design Patterns Used

### 1. **RAII (Resource Acquisition Is Initialization)**
```cpp
class Vehicle {
private:
    std::unique_ptr<IDynamicsModel> dynamics_;
    
public:
    Vehicle(...) {
        dynamics_ = std::make_unique<KinematicModel>();
    }
    // destructor implicitly cleans up dynamics_
};
```

**Benefits**:
- No memory leaks
- Exception-safe
- Clear ownership semantics

### 2. **Strategy Pattern (IDynamicsModel)**
```cpp
class IDynamicsModel {
    virtual void update(VehicleState& state, double dt) = 0;
};

// Interchangeable strategies:
- KinematicModel
- (Future) DynamicModel
- (Future) BicycleModel
```

**Benefits**:
- Runtime algorithm selection
- Easy to add new algorithms
- Dependency injection

### 3. **Strategy Pattern (IACCController)**
```cpp
class IACCController {
    virtual double calculateAcceleration(...) = 0;
};

// Interchangeable controllers:
- SimpleACCController
- (Future) PIDACCController
- (Future) MPC ACCController
```

**Benefits**:
- Testable in isolation
- Easy to compare control algorithms
- Extensible without modification

### 4. **Composition over Inheritance**
```cpp
class Vehicle {
    // HAS-A relationship instead of IS-A
    std::unique_ptr<IDynamicsModel> dynamics_;
};

class ACCSimulation {
    std::vector<std::unique_ptr<Vehicle>> vehicles_;
    std::unique_ptr<IACCController> controller_;
};
```

**Benefits**:
- Flexible combinations
- Easier to understand
- Avoids deep hierarchies

### 5. **Dependency Injection**
```cpp
Vehicle::Vehicle(..., std::unique_ptr<IDynamicsModel> dynamics = nullptr) {
    dynamics_ = dynamics ? std::move(dynamics) 
                         : std::make_unique<KinematicModel>();
}
```

**Benefits**:
- Testable with mock objects
- Loose coupling
- Single responsibility

## State Flow Diagram

```
Initialization:
    ┌─────────────┐
    │ ACCSimulation├─→ Create Vehicle 0 (Lead)
    │ ROS Node    ├─→ Create Vehicle 1 (Ego)
    │             ├─→ Create SimpleACCController
    │             └─→ Start Timer
    └────────────┬┘
                 │
    Every Timer Tick (50ms):
                 │
                 ▼
    ┌──────────────────────────┐
    │ simulationCallback()      │
    │  ├─ ACCSimulation.update()
    │  │   ├─ Calculate ACC cmd
    │  │   │   ├─ Distance error
    │  │   │   └─ Speed error
    │  │   ├─ Update Vehicle 0
    │  │   │   └─ KinematicModel.update()
    │  │   ├─ Update Vehicle 1
    │  │   │   └─ KinematicModel.update()
    │  │   └─ Increment time
    │  │
    │  ├─ createMarkers()
    │  │   ├─ createRoadMarkers()
    │  │   └─ createVehicleMarkers()
    │  │
    │  └─ publish(marker_array)
    └──────────────────────────┘
                 │
                 ▼ (repeat)
```

## Key Interfaces

### IDynamicsModel
```cpp
class IDynamicsModel {
public:
    virtual void update(VehicleState& state, double dt) = 0;
};
```

**Contract**: 
- Input: Vehicle state and time step
- Output: Updated state (modified in-place)
- Implements: Physics simulation

### IACCController
```cpp
class IACCController {
public:
    virtual double calculateAcceleration(const Vehicle& ego,
                                         const Vehicle& lead) = 0;
};
```

**Contract**:
- Input: Two vehicles (following and lead)
- Output: Acceleration command for following vehicle
- Implements: Control logic

## Separation of Concerns

| Component | Responsibility | Dependencies |
|-----------|---|---|
| `VehicleState` | Store vehicle kinematic state | None |
| `IDynamicsModel` | Physics simulation (abstract) | `VehicleState` |
| `KinematicModel` | Kinematic physics | `IDynamicsModel` |
| `Vehicle` | Vehicle entity & state mgmt | `VehicleState`, `IDynamicsModel` |
| `Road` | Road geometry & properties | None |
| `IACCController` | Control logic interface | `Vehicle` |
| `SimpleACCController` | Proportional control | `IACCController`, math |
| `ACCSimulation` | Simulation orchestration | `Vehicle`, `IACCController` |
| `ACCSimulationNode` | ROS integration | `ACCSimulation`, `Road`, rclcpp |

## Extension Points

### Add New Dynamics Model
1. Create class inheriting `IDynamicsModel`
2. Implement `update()` method
3. Pass to Vehicle constructor via dependency injection

```cpp
class AdvancedDynamicsModel : public IDynamicsModel {
    void update(VehicleState& state, double dt) override {
        // Your physics here
    }
};

auto vehicle = std::make_unique<Vehicle>(
    0, 100.0, 0.0, 4.5, 2.0,
    std::make_unique<AdvancedDynamicsModel>()
);
```

### Add New Controller
1. Create class inheriting `IACCController`
2. Implement `calculateAcceleration()` method
3. Set on simulation via `setController()`

```cpp
class PIDController : public IACCController {
    double calculateAcceleration(...) override {
        // Your PID logic here
    }
};

simulation_.setController(std::make_unique<PIDController>());
```

### Add Multi-Vehicle ACC
Modify `ACCSimulation::update()` to handle multiple control laws:

```cpp
void ACCSimulation::update() {
    for (size_t i = 1; i < vehicles_.size(); ++i) {
        double acc = controller_->calculateAcceleration(
            *vehicles_[i],      // ego
            *vehicles_[i - 1]   // lead
        );
        vehicles_[i]->setAcceleration(acc);
    }
    // ... update vehicles ...
}
```

## Memory Model

```
Stack:
┌─────────────────────────────────────┐
│ ACCSimulationNode                   │
│  ├─ ACCSimulation (owned)           │
│  ├─ Road (owned)                    │
│  └─ Publishers, Timers, etc.        │
└─────────────────────────────────────┘
        │
        │ owns
        ▼
    Heap:
┌─────────────────────────────────────┐
│ std::vector<unique_ptr<Vehicle>>    │
│  ├─ unique_ptr<Vehicle 0>           │
│  │   └─ unique_ptr<IDynamicsModel>  │
│  └─ unique_ptr<Vehicle 1>           │
│      └─ unique_ptr<IDynamicsModel>  │
└─────────────────────────────────────┘

┌─────────────────────────────────────┐
│ unique_ptr<IACCController>          │
│  └─ SimpleACCController             │
└─────────────────────────────────────┘
```

**Ownership Chain**: Node → Simulation → Vehicles → Dynamics Models

## Thread Safety

**Current Model**: Single-threaded

- Main ROS thread calls `simulationCallback()`
- All updates are sequential
- No race conditions

**For Multi-threaded Extension**:
- Add mutexes around `vehicles_` vector
- Use thread-safe queues for commands
- Consider executor models in ROS 2

## Performance Characteristics

| Operation | Complexity | Time (2 vehicles) |
|-----------|---|---|
| Vehicle update | O(1) | ~1 μs |
| ACC calculation | O(1) | ~5 μs |
| Marker generation | O(n_vehicles) | ~20 μs |
| Complete cycle | O(n_vehicles) | ~30 μs |

At 20 Hz: ~30 μs/20 cycle = 0.15% CPU utilization

## Error Handling

**Current approach**: Defensive programming

```cpp
// Velocity clamping
velocity = std::max(0.0, velocity);

// Acceleration limits
acceleration = std::clamp(acceleration, -max_decel, max_accel);

// Vector computation safety
distance = std::sqrt(dx*dx + dy*dy);  // Safe, no division
```

**Future improvements**:
- Exception handling for invalid parameters
- Bounds checking for marker array indices
- Validation of control inputs

## Testing Strategy

### Unit Tests (isolated components)
- Vehicle physics with known inputs
- ACC control calculations
- Road geometry calculations

### Integration Tests (composed systems)
- Full simulation update cycles
- Multi-vehicle coordination
- ROS publisher integration

### System Tests (end-to-end)
- Launch file verification
- RViz visualization
- Performance under load

## Documentation Map

- **README.md**: User guide and overview
- **TESTING.md**: Testing scenarios and data analysis
- **ARCHITECTURE.md** (this file): Design decisions and patterns
- **Code comments**: Docstrings for classes and methods
- **Header files**: Interface documentation in `.hpp` files
