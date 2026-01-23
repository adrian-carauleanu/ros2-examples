#include "acc_simulation/vehicle.hpp"
#include <algorithm>

void KinematicModel::update(VehicleState& state, double dt) {
    // Clamp acceleration to limits
    state.acceleration = std::clamp(state.acceleration, 
                                    -max_deceleration_, 
                                    max_acceleration_);

    // Update velocity
    state.velocity += state.acceleration * dt;
    state.velocity = std::max(0.0, state.velocity);  // Prevent negative velocity

    // Update position
    state.position_x += state.velocity * std::cos(state.heading) * dt;
    state.position_y += state.velocity * std::sin(state.heading) * dt;
}

Vehicle::Vehicle(double id, double x, double y, double length, double width,
                 std::unique_ptr<IDynamicsModel> dynamics)
    : state_(id, x, y, 0.0, 0.0, 0.0),
      dynamics_(dynamics ? std::move(dynamics) 
                         : std::make_unique<KinematicModel>()),
      length_(length), width_(width) {}

void Vehicle::update(double dt) {
    dynamics_->update(state_, dt);
}
