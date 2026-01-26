# ACC Simulation - Testing and Examples

## Quick Start

1. **Build the project**:
```bash
cd ~/ros2_ws
source /opt/ros/kilted/setup.bash
colcon build --packages-select acc_simulation
source install/setup.bash
```

2. **Run the simulation**:
```bash
ros2 launch acc_simulation acc_simulation.launch.py
```

This will start:
- The ACC simulation node
- RViz with visualization of the road and vehicles

## Test Scenarios

### Scenario 1: Basic ACC Operation (Default Parameters)
**Parameters**: default (desired_speed=20.0 m/s, distance=30m, kp=0.5, kv=0.3)

**Expected Behavior**:
- Lead vehicle (green) moves at constant 15 m/s
- Ego vehicle (red) starts at 10 m/s
- Ego vehicle accelerates toward 20 m/s
- Distance stabilizes around 30m
- Console logs show convergence over ~15-20 seconds

**Command**:
```bash
ros2 launch acc_simulation acc_simulation.launch.py
```

### Scenario 2: Aggressive Control
**Parameters**: Higher gains for faster response

**Expected Behavior**:
- Ego vehicle responds more aggressively to distance/speed errors
- May oscillate slightly initially
- Settles to desired distance faster

**Command**:
```bash
ros2 launch acc_simulation acc_simulation.launch.py kp:=1.0 kv:=0.6
```

### Scenario 3: Conservative Control
**Parameters**: Lower gains for smooth, gentle control

**Expected Behavior**:
- Ego vehicle responds slowly to errors
- Very smooth acceleration profile
- Longer settling time

**Command**:
```bash
ros2 launch acc_simulation acc_simulation.launch.py kp:=0.2 kv:=0.1
```

### Scenario 4: Close Following
**Parameters**: Reduced desired distance

**Expected Behavior**:
- Ego vehicle maintains closer distance to lead vehicle
- Shorter safe following distance

**Command**:
```bash
ros2 launch acc_simulation acc_simulation.launch.py desired_distance:=20.0
```

### Scenario 5: High-Speed Following
**Parameters**: Higher desired speed

**Expected Behavior**:
- Both vehicles accelerate to higher speed
- Lead vehicle may need to accelerate if it was moving slower
- Demonstrates speed tracking capability

**Command**:
```bash
ros2 launch acc_simulation acc_simulation.launch.py desired_speed:=30.0
```

## RViz Navigation

### View Control:
- **Middle Mouse Button + Drag**: Rotate view
- **Mouse Wheel**: Zoom in/out
- **Right Mouse Button + Drag**: Pan view

### Display Options:
- **Enable/Disable layers**: Expand "MarkerArray" in Displays panel
- **Adjust display properties**: Select each marker type for fine-tuning colors/sizes

### Useful RViz Features:
- **Follow Camera**: Lock view to a moving object
- **Measure Tool**: Calculate distances between vehicles
- **Focus Camera**: Center view on specific objects

## Monitoring and Analysis

### Console Output
The node logs state every 100 simulation steps (~5 seconds at 20 Hz):

```
[INFO] [acc_simulation_node-1]: Time: 5.00s | Lead: x=175.0, v=15.0 m/s | Ego: x=99.5, v=10.5 m/s, a=1.00 m/s²
[INFO] [acc_simulation_node-1]: Time: 10.00s | Lead: x=250.0, v=15.0 m/s | Ego: x=127.2, v=14.2 m/s, a=0.80 m/s²
[INFO] [acc_simulation_node-1]: Time: 15.00s | Lead: x=325.0, v=15.0 m/s | Ego: x=160.8, v=17.5 m/s, a=-0.50 m/s²
```

**Key Metrics**:
- **Time**: Simulation elapsed time
- **Lead position**: X coordinate of lead vehicle
- **Lead velocity**: Constant or variable
- **Ego position**: X coordinate of following vehicle
- **Ego velocity**: Should converge toward desired speed
- **Ego acceleration**: Control output from ACC algorithm

### Distance Analysis
Calculate following distance:
```
Distance = Lead_Position - Ego_Position - Lead_Length - Ego_Length
Expected ≈ desired_distance (30m default)
```

## Code Testing

### Running Unit Tests (Future)
Once unit tests are implemented:
```bash
colcon test --packages-select acc_simulation
```

### Manual Integration Testing

**Test 1: Verify Vehicle Creation**
```cpp
// Check in code
auto vehicle = std::make_unique<Vehicle>(0, 100.0, 0.0);
assert(vehicle->getId() == 0);
assert(vehicle->getPositionX() == 100.0);
assert(vehicle->getVelocity() == 0.0);
```

**Test 2: Verify ACC Calculation**
```cpp
Vehicle ego(1, 0.0, 0.0);
Vehicle lead(0, 50.0, 0.0);
ego.setVelocity(10.0);
lead.setVelocity(15.0);

SimpleACCController controller(20.0, 30.0);
double accel = controller.calculateAcceleration(ego, lead);
// Should be positive (accelerate toward target speed/distance)
```

**Test 3: Verify Dynamics Update**
```cpp
Vehicle vehicle(0, 0.0, 0.0);
vehicle.setAcceleration(1.0);
vehicle.update(1.0);  // 1 second update
// Position should be approximately 0.5m (0.5 * 1.0 * 1.0^2)
// Velocity should be 1.0 m/s
```

## Performance Tuning Guide

### Finding Optimal Gains

**Start Conservative**:
- kp = 0.3, kv = 0.2
- Observe response and overshoot

**If Response Too Slow**:
- Increase kp (0.5 → 0.7) or kv (0.2 → 0.4)
- Test one at a time

**If Oscillating/Overshooting**:
- Decrease kp or kv
- Increase damping by increasing kv relative to kp

**Good Balance**:
- Small overshoot acceptable
- Settling time < 10 seconds
- No oscillation after settling

### Distance Gain Tuning
Distance error = desired_distance - actual_distance

- **Too far**: kp too low, acceleration insufficient
- **Too close**: kp too high, deceleration excessive
- Target: Smooth convergence to desired distance

### Speed Gain Tuning
Speed error = desired_speed - current_speed

- **Doesn't reach target**: kv too low
- **Overshoots target**: kv too high
- Target: Reaches desired speed without overshoot

## Data Logging

### Redirect Output to File
```bash
ros2 launch acc_simulation acc_simulation.launch.py > acc_sim.log 2>&1
```

### Parse Simulation Data
Extract metrics from logs:
```bash
grep "\[INFO\]" acc_sim.log | grep "Lead:" | awk '{print $NF}'
```

### Visualization in Python
```python
import re
times = []
positions = []
with open('acc_sim.log') as f:
    for line in f:
        if 'Lead:' in line:
            match = re.search(r'Time: ([\d.]+)s.*Ego: x=([\d.]+)', line)
            if match:
                times.append(float(match.group(1)))
                positions.append(float(match.group(2)))

import matplotlib.pyplot as plt
plt.plot(times, positions)
plt.xlabel('Time (s)')
plt.ylabel('Position (m)')
plt.title('Ego Vehicle Position Over Time')
plt.show()
```

## Troubleshooting

### Issue: Vehicle doesn't accelerate
**Solution**: Check kv gain is positive and large enough

### Issue: Vehicle oscillates around target speed
**Solution**: Reduce kp and kv, increase their ratio balance

### Issue: Distance grows unbounded
**Solution**: Increase kp (distance control gain)

### Issue: RViz doesn't show markers
**Solution**: 
- Check topic: `ros2 topic list | grep marker`
- Verify subscription in RViz display settings
- Check frame ID matches RViz "Fixed Frame" (should be "map")

### Issue: Simulation runs slowly
**Solution**: Reduce update_frequency or RViz rendering

## Next Steps

1. **Extend with multiple vehicles**: Add 3-4 vehicles in a platoon
2. **Implement PID controller**: Replace simple proportional with full PID
3. **Add data logging**: Record states to file for analysis
4. **Create test scenarios**: Predefined lead vehicle profiles
5. **Add collision detection**: Safety checks for minimum distance
6. **Lane changes**: Implement lane changing behavior
7. **Traffic scenarios**: Multiple platoons interacting
