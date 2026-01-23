#ifndef ACC_SIMULATION__ROAD_HPP_
#define ACC_SIMULATION__ROAD_HPP_

/**
 * @brief Represents road configuration for visualization and constraints
 * 
 * Encapsulates road properties using RAII
 */
class Road {
private:
    double length_;      // Length of road in meters
    double width_;       // Width of one lane in meters
    double num_lanes_;   // Number of lanes
    double lane_marking_distance_;  // Distance between lane markings

public:
    Road(double length = 500.0, double width = 3.5, double num_lanes = 2.0,
         double lane_marking_distance = 10.0)
        : length_(length), width_(width), num_lanes_(num_lanes),
          lane_marking_distance_(lane_marking_distance) {}

    ~Road() = default;

    // Getters
    double getLength() const { return length_; }
    double getWidth() const { return width_; }
    double getNumLanes() const { return num_lanes_; }
    double getLaneMarkingDistance() const { return lane_marking_distance_; }

    /**
     * @brief Get the Y position of a lane center
     * @param lane Lane number (0, 1, 2, ...)
     * @return Y coordinate of lane center
     */
    double getLaneYPosition(int lane) const {
        return lane * width_ - (num_lanes_ - 1) * width_ / 2.0;
    }
};

#endif  // ACC_SIMULATION__ROAD_HPP_
