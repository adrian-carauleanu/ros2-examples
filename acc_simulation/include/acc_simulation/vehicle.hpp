#ifndef ACC_SIMULATION__VEHICLE_HPP_
#define ACC_SIMULATION__VEHICLE_HPP_

#include <cmath>
#include <memory>

/**
 * @brief Represents the state of a vehicle in 2D space
 */
class VehicleState {
public:
    double id;
    double position_x;
    double position_y;
    double velocity;
    double acceleration;
    double heading;  // radians

    VehicleState(double id = 0.0, double x = 0.0, double y = 0.0, 
                 double vel = 0.0, double accel = 0.0, double heading = 0.0)
        : id(id), position_x(x), position_y(y), velocity(vel), 
          acceleration(accel), heading(heading) {}
};

/**
 * @brief Interface for vehicle dynamics model
 */
class IDynamicsModel {
public:
    virtual ~IDynamicsModel() = default;
    
    /**
     * @brief Update vehicle state based on time step
     * @param state Current vehicle state
     * @param dt Time step in seconds
     */
    virtual void update(VehicleState& state, double dt) = 0;
};

/**
 * @brief Simple kinematic vehicle model
 */
class KinematicModel : public IDynamicsModel {
private:
    double max_acceleration_;
    double max_deceleration_;

public:
    KinematicModel(double max_accel = 5.0, double max_decel = 8.0)
        : max_acceleration_(max_accel), max_deceleration_(max_decel) {}

    void update(VehicleState& state, double dt) override;
};

/**
 * @brief Vehicle class encapsulating position, velocity, and dynamics
 */
class Vehicle {
private:
    VehicleState state_;
    std::unique_ptr<IDynamicsModel> dynamics_;
    const double length_;
    const double width_;

public:
    Vehicle(double id, double x, double y, double length = 4.5, double width = 2.0,
            std::unique_ptr<IDynamicsModel> dynamics = nullptr);

    ~Vehicle() = default;  // RAII: dynamics_ is automatically cleaned up

    // Getters
    const VehicleState& g
    double getLength() const { return length_; }
    double getWidth() const { return width_; }
    double getId() const { return state_.id; }
    double getPositionX() const { return state_.position_x; }
    double getPositionY() const { return state_.position_y; }
    double getVelocity() const { return state_.velocity; }
    double getHeading() const { return state_.heading; }

    // Setters
    void setVelocity(double vel) { state_.velocity = std::max(0.0, vel); }
    void setAcceleration(double accel) { state_.acceleration = accel; }
    void setHeading(double heading) { state_.heading = heading; }
    void setPosition(double x, double y) {
        state_.position_x = x;
        state_.position_y = y;
    }

    /**
     * @brief Update vehicle dynamics for a time step
     * @param dt Time step in seconds
     */
    void update(double dt);
};

#endif  // ACC_SIMULATION__VEHICLE_HPP_
