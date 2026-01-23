#ifndef ACC_SIMULATION__ACC_CONTROLLER_HPP_
#define ACC_SIMULATION__ACC_CONTROLLER_HPP_

#include "vehicle.hpp"
#include <memory>
#include <vector>

/**
 * @brief Interface for ACC control logic
 */
class IACCController {
public:
    virtual ~IACCController() = default;

    /**
     * @brief Calculate acceleration for the ego vehicle
     * @param ego_vehicle The following vehicle
     * @param lead_vehicle The vehicle ahead
     * @return Target acceleration
     */
    virtual double calculateAcceleration(const Vehicle& ego_vehicle,
                                         const Vehicle& lead_vehicle) = 0;
};

/**
 * @brief Implements adaptive cruise control
 */
class SimpleACCController : public IACCController {
private:
    double desired_speed_;
    double desired_distance_;  // m
    double kp_;  // proportional gain for distance control
    double kv_;  // proportional gain for speed control
    double max_acceleration_;
    double max_deceleration_;

public:
    SimpleACCController(double desired_speed = 20.0,  // m/s
                        double desired_distance = 30.0,  // m
                        double kp = 0.5,
                        double kv = 0.3,
                        double max_accel = 5.0,
                        double max_decel = 8.0)
        : desired_speed_(desired_speed),
          desired_distance_(desired_distance),
          kp_(kp),
          kv_(kv),
          max_acceleration_(max_accel),
          max_deceleration_(max_decel) {}

    double calculateAcceleration(const Vehicle& ego_vehicle,
                                 const Vehicle& lead_vehicle) override;

    // Setters for tuning
    void setDesiredSpeed(double speed) { desired_speed_ = speed; }
    void setDesiredDistance(double distance) { desired_distance_ = distance; }
    void setGains(double kp, double kv) { kp_ = kp; kv_ = kv; }
};

/**
 * @brief Manages multiple vehicles and ACC control
 */
class ACCSimulation {
private:
    std::vector<std::unique_ptr<Vehicle>> vehicles_;
    std::unique_ptr<IACCController> controller_;
    double simulation_time_;
    double time_step_;

public:
    ACCSimulation(double time_step = 0.05)
        : simulation_time_(0.0), time_step_(time_step) {}

    ~ACCSimulation() = default;

    /**
     * @brief Add a vehicle to the simulation
     * @param vehicle Vehicle to add (takes ownership)
     */
    void addVehicle(std::unique_ptr<Vehicle> vehicle);

    /**
     * @brief Remove the last vehicle from the simulation
     * @return True if a vehicle was removed, false if no vehicles to remove
     */
    bool removeLastVehicle();

    /**
     * @brief Set the ACC controller
     * @param controller ACC controller (takes ownership)
     */
    void setController(std::unique_ptr<IACCController> controller);

    /**
     * @brief Update simulation for one time step
     */
    void update();

    // Getters
    const std::vector<std::unique_ptr<Vehicle>>& getVehicles() const {
        return vehicles_;
    }

    double getSimulationTime() const { return simulation_time_; }
    size_t getVehicleCount() const { return vehicles_.size(); }

    void SetDesiredDistance(double distance) {
        if (controller_) {
            if (auto acc_controller = dynamic_cast<SimpleACCController*>(controller_.get())) {
                acc_controller->setDesiredDistance(distance);
            }
        }
    }
};

#endif  // ACC_SIMULATION__ACC_CONTROLLER_HPP_
