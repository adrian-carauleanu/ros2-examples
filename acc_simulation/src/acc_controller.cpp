#include "acc_simulation/acc_controller.hpp"
#include <cmath>
#include <algorithm>

double SimpleACCController::calculateAcceleration(const Vehicle& ego_vehicle,
                                                   const Vehicle& lead_vehicle) {
    // Calculate distance between vehicles
    double dx = lead_vehicle.getPositionX() - ego_vehicle.getPositionX();
    double dy = lead_vehicle.getPositionY() - ego_vehicle.getPositionY();
    double distance = std::sqrt(dx * dx + dy * dy);

    // Distance error (positive means too far, negative means too close)
    double distance_error = distance - ego_vehicle.getLength() 
                           - lead_vehicle.getLength() - desired_distance_;

    // Velocity error (positive means ego is slower than desired)
    double velocity_error = desired_speed_ - ego_vehicle.getVelocity();

    // Combined control law
    double acceleration = kp_ * distance_error + kv_ * velocity_error;

    // Clamp acceleration to limits
    acceleration = std::clamp(acceleration, -max_deceleration_, max_acceleration_);

    return acceleration;
}

void ACCSimulation::addVehicle(std::unique_ptr<Vehicle> vehicle) {
    vehicles_.push_back(std::move(vehicle));
}

void ACCSimulation::setController(std::unique_ptr<IACCController> controller) {
    controller_ = std::move(controller);
}

void ACCSimulation::update() {
    if (!controller_ || vehicles_.size() < 2) {
        // Just update vehicle dynamics without ACC control
        for (auto& vehicle : vehicles_) {
            vehicle->update(time_step_);
        }
        simulation_time_ += time_step_;
        return;
    }

    // Assume first vehicle is lead, second is ego for ACC control
    // Calculate acceleration for ego vehicle
    double acc = controller_->calculateAcceleration(*vehicles_[1], *vehicles_[0]);
    vehicles_[1]->setAcceleration(acc);

    // Update all vehicles
    for (auto& vehicle : vehicles_) {
        vehicle->update(time_step_);
    }

    simulation_time_ += time_step_;
}
