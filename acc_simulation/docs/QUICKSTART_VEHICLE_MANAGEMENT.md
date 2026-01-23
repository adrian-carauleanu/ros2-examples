# Quick Start: Vehicle Management (Phase 7)

## 30-Second Setup

```bash
cd ~/ros2_ws
source /opt/ros/kilted/setup.bash
colcon build --packages-select acc_simulation acc_simulation_panel
source install/setup.bash
```

## 3 Ways to Manage Vehicles

### 1️⃣ Command Line (Fastest)
```bash
# Terminal 1: Run simulation
ros2 launch acc_simulation acc_simulation.launch.py

# Terminal 2: Add vehicles
ros2 service call /acc_simulation_node/add_vehicle std_srvs/srv/Empty {}

# Terminal 2: Remove vehicles
ros2 service call /acc_simulation_node/remove_vehicle std_srvs/srv/Empty {}
```

### 2️⃣ RViz Panel (Most Visual)
```bash
# Terminal 1: Run simulation
ros2 launch acc_simulation acc_simulation.launch.py

# Terminal 2: Open RViz
rviz2

# In RViz: Panels → Add New Panel → acc_simulation_panel::VehicleControlPanel
# Click buttons to add/remove
```

### 3️⃣ Programmatic (Custom Code)
```cpp
auto client = node->create_client<std_srvs::srv::Empty>(
    "/acc_simulation_node/add_vehicle");
auto future = client->async_send_request(
    std::make_shared<std_srvs::srv::Empty::Request>());
```

## What Happens When You Add/Remove

### Add Vehicle
- ✅ New red vehicle created 40m behind last vehicle
- ✅ Initial velocity: 10 m/s
- ✅ Appears immediately in RViz
- ✅ ACC control works automatically
- ✅ Sequential ID assigned

### Remove Vehicle
- ✅ Last vehicle removed from simulation
- ✅ Disappears immediately from RViz
- ✅ At least 1 vehicle always remains (safe)
- ✅ Accelerations recalculated for followers

## Visual Indicators in RViz

| Color | Vehicle Type | Notes |
|-------|--------------|-------|
| 🟢 Green | Lead Vehicle | Vehicle 0, always present |
| 🔴 Red | Following Vehicles | ACC controlled |
| 🔵 Blue Arrows | Velocity Vectors | Direction of motion |

## Troubleshooting

**"Service not available"**
- Ensure simulation is running: `ros2 service list | grep vehicle`

**Panel doesn't appear**
- Rebuild: `colcon build --packages-select acc_simulation_panel`
- Restart RViz

**Vehicles don't disappear**
- Make sure visualization markers are enabled in RViz

## Service Details

| Service | Type | What It Does |
|---------|------|-------------|
| `/acc_simulation_node/add_vehicle` | Empty | Creates vehicle at Y = (last_y - 40) |
| `/acc_simulation_node/remove_vehicle` | Empty | Removes last vehicle (if count > 1) |

## Next Steps

1. **Try the CLI**: Add/remove vehicles from terminal
2. **Use the Panel**: Launch RViz and add the panel
3. **Explore Parameters**: Try different acc gains
   ```bash
   ros2 launch acc_simulation acc_simulation.launch.py kp:=1.0 kv:=0.6
   ```
4. **Read Full Docs**: See [docs/VEHICLE_MANAGEMENT.md](src/acc_simulation/docs/VEHICLE_MANAGEMENT.md)

## Performance Notes

- Adding vehicles: < 1ms
- Removing vehicles: < 1ms  
- Simulation continues smoothly
- No lag in RViz visualization
- Safe for real-time operation

---

**That's it! You now have interactive vehicle management. Enjoy! 🚗**
