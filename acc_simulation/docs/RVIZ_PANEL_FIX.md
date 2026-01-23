# RViz Panel Fix - Setup Instructions

## The Issue
The RViz panel wasn't being found because the library wasn't being built. **This has been fixed.**

## What Was Fixed
1. **CMakeLists.txt** - Updated to properly build the shared library (.so)
2. **Library Installation** - Now correctly installs to `lib/libacc_simulation_panel_lib.so`
3. **Plugin Registration** - plugins.xml is properly installed to `share/acc_simulation_panel/`

## Proper Setup Steps (Do This)

### Step 1: Source Environment First
```bash
cd ~/ros2_ws
source install/setup.bash
```

### Step 2: Launch Simulation (Terminal 1)
```bash
ros2 launch acc_simulation acc_simulation.launch.py
```

### Step 3: Launch RViz (Terminal 2)
```bash
source ~/ros2_ws/install/setup.bash
rviz2
```

**IMPORTANT**: You MUST source `install/setup.bash` before launching RViz. This is critical for RViz to find the plugin.

### Step 4: Add the Panel
1. In RViz menu: **Panels** → **Add New Panel**
2. Find and select: **acc_simulation_panel::VehicleControlPanel**
3. The panel appears on the right side with "Add Vehicle" and "Remove Vehicle" buttons

## Verify Plugin is Discoverable
```bash
source ~/ros2_ws/install/setup.bash
ros2 plugin list | grep acc_simulation_panel
# Should show: acc_simulation_panel (under rviz_common::Panel)
```

## If Panel Still Doesn't Appear

### Check 1: Verify Library Exists
```bash
ls -la ~/ros2_ws/install/acc_simulation_panel/lib/libacc_simulation_panel_lib.so
# Should exist (if not, rebuild: colcon build --packages-select acc_simulation_panel)
```

### Check 2: Verify plugins.xml Exists
```bash
cat ~/ros2_ws/install/acc_simulation_panel/share/acc_simulation_panel/plugins.xml
# Should contain plugin definition
```

### Check 3: Check RViz Plugin Path
```bash
export | grep AMENT_PREFIX_PATH
# Should include ~/ros2_ws/install/acc_simulation_panel
```

### Check 4: Rebuild and Clear Cache
```bash
rm -rf ~/.config/ros2_gui_cpp.log
rm -rf ~/.local/share/rviz2/
colcon build --packages-select acc_simulation_panel --symlink-install
source install/setup.bash
rviz2
```

## Quick Test

Run these exact commands:

```bash
# Terminal 1
cd ~/ros2_ws
source install/setup.bash
ros2 launch acc_simulation acc_simulation.launch.py

# Terminal 2 (wait for Terminal 1 to fully start)
source ~/ros2_ws/install/setup.bash
rviz2
```

Then in RViz: **Panels** → **Add New Panel** → Look for **acc_simulation_panel::VehicleControlPanel**

---

**Note**: The key difference from before is that we now have a proper SHARED library with all dependencies linked. RViz can now find and load the plugin.
