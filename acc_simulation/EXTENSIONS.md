# Extension Guide - Adding Features to ACC Simulation

This guide shows how to extend the ACC simulation with new features following the existing architecture.

## Example 1: Adding a PID Controller

### Step 1: Create PID Header File

Create `include/acc_simulation/pid_acc_controller.hpp`:

```cpp
#ifndef ACC_SIMULATION__PID_ACC_CONTROLLER_HPP_
#define ACC_SIMULATION__PID_ACC_CONTROLLER_HPP_

#include "acc_controller.hpp"
#include <queue>
#include <deque>

/**
 * @brief PID-based Adaptive Cruise Control
 * 
 * Uses integral and derivative terms for better control
 */
class PIDACCController : public IACCController {
private:
    double desired_speed_;
    double desired_distance_;
    
    // PID gains
    double kp_;  // Proportional
    double ki_;  // Integral
    double kd_;  // Derivative
    
    // State for integral and derivative
    double error_integral_;
    double last_error_;
    std::deque<double> error_history_;  // For derivative
    
    double max_acceleration_;
    double max_deceleration_;

public:
    PIDACCController(double desired_speed = 20.0,
                     double desired_distance = 30.0,
                     double kp = 0.5,
                     double ki = 0.1,
                     double kd = 0.3,
                     double max_accel = 5.0,
                     double max_decel = 8.0)
        : desired_speed_(desired_speed),
          desired_distance_(desired_distance),
          kp_(kp), ki_(ki), kd_(kd),
          error_integral_(0.0), last_error_(0.0),
          max_acceleration_(max_accel),
          max_deceleration_(max_decel) {}

    double calculateAcceleration(const Vehicle& ego_vehicle,
                                 const Vehicle& lead_vehicle) override;

    void reset() {
        error_integral_ = 0.0;
        last_error_ = 0.0;
        error_history_.clear();
    }

    void setGains(double kp, double ki, double kd) {
        kp_ = kp; ki_ = ki; kd_ = kd;
    }
};

#endif  // ACC_SIMULATION__PID_ACC_CONTROLLER_HPP_
```

### Step 2: Create PID Implementation

Create `src/pid_acc_controller.cpp`:

```cpp
#include "acc_simulation/pid_acc_controller.hpp"
#include <cmath>
#include <algorithm>

double PIDACCController::calculateAcceleration(const Vehicle& ego_vehicle,
                                               const Vehicle& lead_vehicle) {
    // Calculate distance
    double dx = lead_vehicle.getPositionX() - ego_vehicle.getPositionX();
    double dy = lead_vehicle.getPositionY() - ego_vehicle.getPositionY();
    double distance = std::sqrt(dx * dx + dy * dy);

    // Distance error (positive = too far)
    double distance_error = distance - ego_vehicle.getLength() 
                           - lead_vehicle.getLength() - desired_distance_;

    // Velocity error
    double velocity_error = desired_speed_ - ego_vehicle.getVelocity();

    // Combined error
    double total_error = 0.5 * distance_error + 0.5 * velocity_error;

    // Integral term
    error_integral_ += total_error * 0.05;  // 0.05 is dt
    error_integral_ = std::clamp(error_integral_, -10.0, 10.0);

    // Derivative term
    double error_derivative = (total_error - last_error_) / 0.05;
    last_error_ = total_error;

    // PID control law
    double acceleration = kp_ * total_error + 
                         ki_ * error_integral_ + 
                         kd_ * error_derivative;

    // Clamp acceleration
    acceleration = std::clamp(acceleration, -max_deceleration_, max_acceleration_);

    return acceleration;
}
```

### Step 3: Update CMakeLists.txt

Add to the library sources:

```cmake
add_library(acc_simulation_lib
  src/vehicle.cpp
  src/acc_controller.cpp
  src/pid_acc_controller.cpp  # ADD THIS LINE
  src/acc_simulation_node.cpp
)
```

### Step 4: Use in Node

In `acc_simulation_node.cpp`, you can now use it:

```cpp
// Instead of:
auto controller = std::make_unique<SimpleACCController>(...);

// Use PID:
auto controller = std::make_unique<PIDACCController>(
    desired_speed, desired_distance, 0.5, 0.1, 0.3);
```

---

## Example 2: Adding a Third Vehicle

### Modify ACCSimulation for Multiple Vehicles

Update `acc_controller.cpp` to handle vehicle chains:

```cpp
void ACCSimulation::update() {
    if (!controller_ || vehicles_.size() < 1) {
        for (auto& vehicle : vehicles_) {
            vehicle->update(time_step_);
        }
        simulation_time_ += time_step_;
        return;
    }

    // Control each vehicle based on the one in front
    for (size_t i = 1; i < vehicles_.size(); ++i) {
        double acc = controller_->calculateAcceleration(
            *vehicles_[i],      // ego
            *vehicles_[i - 1]   // lead
        );
        vehicles_[i]->setAcceleration(acc);
    }

    // Update all vehicles
    for (auto& vehicle : vehicles_) {
        vehicle->update(time_step_);
    }

    simulation_time_ += time_step_;
}
```

### Add Vehicles in Node Constructor

In `acc_simulation_node.cpp`:

```cpp
ACCSimulationNode::ACCSimulationNode(const std::string& node_name)
    : rclcpp::Node(node_name), ...
{
    // ... parameters ...

    // Create three vehicles in a platoon
    auto lead_vehicle = std::make_unique<Vehicle>(
        0, 150.0, road_.getLaneYPosition(0), 4.5, 2.0);
    lead_vehicle->setVelocity(15.0);

    auto ego_vehicle_1 = std::make_unique<Vehicle>(
        1, 100.0, road_.getLaneYPosition(0), 4.5, 2.0);
    ego_vehicle_1->setVelocity(10.0);

    auto ego_vehicle_2 = std::make_unique<Vehicle>(
        2, 50.0, road_.getLaneYPosition(0), 4.5, 2.0);
    ego_vehicle_2->setVelocity(8.0);

    simulation_.addVehicle(std::move(lead_vehicle));
    simulation_.addVehicle(std::move(ego_vehicle_1));
    simulation_.addVehicle(std::move(ego_vehicle_2));

    auto controller = std::make_unique<SimpleACCController>(...);
    simulation_.setController(std::move(controller));
}
```

### Update Marker Colors

In `createVehicleMarkers()`:

```cpp
// Color based on position in platoon
if (i == 0) {  // Lead
    vehicle_marker.color.r = 0.0;
    vehicle_marker.color.g = 1.0;  // Green
    vehicle_marker.color.b = 0.0;
} else if (i == 1) {  // Middle
    vehicle_marker.color.r = 1.0;
    vehicle_marker.color.g = 1.0;  // Yellow
    vehicle_marker.color.b = 0.0;
} else {  // Rear
    vehicle_marker.color.r = 1.0;
    vehicle_marker.color.g = 0.0;  // Red
    vehicle_marker.color.b = 0.0;
}
```

---

## Example 3: Adding a Lead Vehicle Profile

### Create Lead Vehicle Behavior

Create `include/acc_simulation/lead_profile.hpp`:

```cpp
#ifndef ACC_SIMULATION__LEAD_PROFILE_HPP_
#define ACC_SIMULATION__LEAD_PROFILE_HPP_

/**
 * @brief Defines speed profile for lead vehicle
 */
class ILeadProfile {
public:
    virtual ~ILeadProfile() = default;
    
    /**
     * @brief Get desired speed at given time
     * @param time Current simulation time
     * @return Target speed in m/s
     */
    virtual double getTargetSpeed(double time) = 0;
};

/**
 * @brief Constant speed profile
 */
class ConstantSpeedProfile : public ILeadProfile {
private:
    double speed_;

public:
    explicit ConstantSpeedProfile(double speed = 15.0) : speed_(speed) {}

    double getTargetSpeed(double time) override { return speed_; }
};

/**
 * @brief Speed ramp profile (linear change)
 */
class RampSpeedProfile : public ILeadProfile {
private:
    double initial_speed_;
    double final_speed_;
    double ramp_duration_;

public:
    RampSpeedProfile(double initial = 15.0, double final = 25.0, 
                     double duration = 10.0)
        : initial_speed_(initial), final_speed_(final), 
          ramp_duration_(duration) {}

    double getTargetSpeed(double time) override;
};

/**
 * @brief Step speed profile (sudden changes)
 */
class StepSpeedProfile : public ILeadProfile {
private:
    std::vector<std::pair<double, double>> steps_;  // (time, speed)

public:
    void addStep(double time, double speed) {
        steps_.push_back({time, speed});
    }

    double getTargetSpeed(double time) override;
};

#endif  // ACC_SIMULATION__LEAD_PROFILE_HPP_
```

### Use in Node

Modify vehicle creation in node:

```cpp
auto lead_vehicle = std::make_unique<Vehicle>(0, 150.0, 0.0);

// Use a ramp profile (accelerate from 15 to 25 m/s over 30s)
auto profile = std::make_unique<RampSpeedProfile>(15.0, 25.0, 30.0);

// Store profile and update in callback
profile_ = std::move(profile);
lead_vehicle->setVelocity(profile_->getTargetSpeed(0.0));
```

---

## Example 4: Adding Multi-Lane Vehicles

### Update Road to Support Lane Changes

```cpp
class Road {
    // ... existing code ...
    
    /**
     * @brief Get total road width
     */
    double getTotalWidth() const { 
        return width_ * num_lanes_; 
    }

    /**
     * @brief Check if adjacent lane is available
     */
    bool isLaneAvailable(int lane) const {
        return lane >= 0 && lane < num_lanes_;
    }
};
```

### Add Lane Change Support

```cpp
class Vehicle {
    // ... existing members ...
    
    int current_lane_;
    int target_lane_;

public:
    void changeLane(int lane) { target_lane_ = lane; }
    int getLane() const { return current_lane_; }
};
```

### Update Position Update

```cpp
void Vehicle::update(double dt) {
    // Update heading for lane position
    if (target_lane_ != current_lane_) {
        // Gradually move to target lane
        state_.position_y += (target_lane_ - current_lane_) * width * dt;
    }
    
    dynamics_->update(state_, dt);
}
```

---

## Example 5: Adding Adaptive Parameters

### Create Parameter Tuner

```cpp
class ACCParameterTuner {
private:
    SimpleACCController& controller_;
    double kp_base_, kv_base_;

public:
    explicit ACCParameterTuner(SimpleACCController& ctrl)
        : controller_(ctrl), kp_base_(0.5), kv_base_(0.3) {}

    /**
     * @brief Adjust gains based on vehicle speed
     * Speed-dependent gain scheduling
     */
    void updateForSpeed(double speed) {
        // Increase gains at higher speeds for faster response
        double speed_factor = std::max(0.5, std::min(1.5, speed / 20.0));
        controller_.setGains(kp_base_ * speed_factor, 
                           kv_base_ * speed_factor);
    }

    /**
     * @brief Adjust gains based on distance error
     * Adaptive response based on following distance
     */
    void updateForDistance(double distance_error) {
        if (std::abs(distance_error) > 50.0) {
            // Very far: reduce gains (smooth response)
            controller_.setGains(kp_base_ * 0.7, kv_base_ * 0.7);
        } else if (std::abs(distance_error) < 5.0) {
            // Very close: increase gains (aggressive response)
            controller_.setGains(kp_base_ * 1.3, kv_base_ * 1.3);
        }
    }
};
```

---

## Example 6: Adding Simulation Events

### Event System

```cpp
class SimulationEvent {
public:
    virtual ~SimulationEvent() = default;
    virtual void execute(ACCSimulation& sim, double time) = 0;
};

class VehicleAccelerationEvent : public SimulationEvent {
private:
    int vehicle_id_;
    double target_acceleration_;

public:
    VehicleAccelerationEvent(int id, double accel)
        : vehicle_id_(id), target_acceleration_(accel) {}

    void execute(ACCSimulation& sim, double time) override {
        // Set acceleration for specific vehicle
    }
};

class EventScheduler {
private:
    std::vector<std::pair<double, std::unique_ptr<SimulationEvent>>> events_;

public:
    void scheduleEvent(double time, std::unique_ptr<SimulationEvent> event) {
        events_.push_back({time, std::move(event)});
    }

    void checkAndExecute(ACCSimulation& sim, double current_time) {
        for (auto& [time, event] : events_) {
            if (std::abs(time - current_time) < 0.01) {
                event->execute(sim, current_time);
            }
        }
    }
};
```

---

## Testing Your Extensions

### Build Updated Code

```bash
cd ~/ros2_ws
colcon build --packages-select acc_simulation
source install/setup.bash
```

### Test New Components

```cpp
// Simple unit test
void test_pid_controller() {
    Vehicle ego(1, 0.0, 0.0);
    Vehicle lead(0, 50.0, 0.0);
    ego.setVelocity(10.0);
    lead.setVelocity(15.0);

    PIDACCController controller(20.0, 30.0);
    double accel = controller.calculateAcceleration(ego, lead);
    
    assert(accel > 0);  // Should accelerate
    std::cout << "PID test passed!" << std::endl;
}
```

### Run Simulation

```bash
ros2 launch acc_simulation acc_simulation.launch.py
```

---

## Best Practices for Extensions

1. **Always inherit from abstract base classes** (IACCController, IDynamicsModel)
2. **Use smart pointers** for automatic memory management
3. **Keep single responsibility** - one class, one job
4. **Document interfaces** with clear contracts
5. **Test components independently** before integration
6. **Use dependency injection** to pass dependencies
7. **Avoid circular dependencies** between modules
8. **Update CMakeLists.txt** when adding new files
9. **Keep backward compatibility** with existing code
10. **Add logging** for debugging new features

---

## Common Extension Patterns

| Pattern | Use Case | Example |
|---------|----------|---------|
| **Strategy Pattern** | Different algorithms | Multiple controller types |
| **Decorator Pattern** | Add features to existing | Wrap controller with logging |
| **Observer Pattern** | Notify on changes | Event system for events |
| **Factory Pattern** | Create objects | Vehicle factory |
| **Template Method** | Common algorithm structure | Base controller update loop |

---

## Performance Tips

- Use `std::vector<std::unique_ptr<>>` instead of `std::vector<std::shared_ptr<>>`
- Avoid virtual function calls in tight loops (move to callbacks)
- Cache calculated values (distance, errors)
- Use move semantics when passing large objects
- Profile with `perf` or `valgrind` for bottlenecks
