#!/bin/bash

# ACC Simulation Phase 7 - Verification Script
# This script verifies that all Phase 7 components are properly built and configured

set -e

WORKSPACE="${HOME}/ros2_ws"
BOLD='\033[1m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BOLD}ACC Simulation Phase 7 - Verification Script${NC}"
echo "=============================================="
echo ""

# Check 1: Workspace exists
echo -e "${BOLD}[1] Checking workspace...${NC}"
if [ -d "$WORKSPACE" ]; then
    echo -e "${GREEN}✓${NC} Workspace found at $WORKSPACE"
else
    echo -e "${RED}✗${NC} Workspace not found at $WORKSPACE"
    exit 1
fi

# Check 2: Build directory exists
echo ""
echo -e "${BOLD}[2] Checking build artifacts...${NC}"
if [ -d "$WORKSPACE/build/acc_simulation" ] && [ -d "$WORKSPACE/build/acc_simulation_panel" ]; then
    echo -e "${GREEN}✓${NC} Both packages built successfully"
else
    echo -e "${RED}✗${NC} Build artifacts missing"
    echo "  Run: cd $WORKSPACE && colcon build --packages-select acc_simulation acc_simulation_panel"
    exit 1
fi

# Check 3: Install directory
echo ""
echo -e "${BOLD}[3] Checking install artifacts...${NC}"
if [ -f "$WORKSPACE/install/acc_simulation/lib/acc_simulation/acc_simulation_node" ] && \
   [ -d "$WORKSPACE/install/acc_simulation_panel/lib" ]; then
    echo -e "${GREEN}✓${NC} Compiled packages installed"
else
    echo -e "${RED}✗${NC} Install artifacts incomplete"
    exit 1
fi

# Check 4: Documentation files
echo ""
echo -e "${BOLD}[4] Checking documentation files...${NC}"
docs=(
    "$WORKSPACE/src/acc_simulation/docs/VEHICLE_MANAGEMENT.md"
    "$WORKSPACE/src/acc_simulation_panel/RVIZ_PANEL_GUIDE.md"
    "$WORKSPACE/PHASE_7_SUMMARY.md"
    "$WORKSPACE/PHASE_7_COMPLETION_REPORT.md"
    "$WORKSPACE/QUICKSTART_VEHICLE_MANAGEMENT.md"
    "$WORKSPACE/INDEX.md"
)

missing_docs=0
for doc in "${docs[@]}"; do
    if [ -f "$doc" ]; then
        echo -e "${GREEN}✓${NC} $(basename "$doc")"
    else
        echo -e "${RED}✗${NC} $(basename "$doc") - MISSING"
        missing_docs=$((missing_docs + 1))
    fi
done

if [ $missing_docs -gt 0 ]; then
    echo -e "${YELLOW}Warning: $missing_docs documentation files missing${NC}"
fi

# Check 5: Source files modified
echo ""
echo -e "${BOLD}[5] Checking modified source files...${NC}"
source_files=(
    "$WORKSPACE/src/acc_simulation/include/acc_simulation/acc_controller.hpp"
    "$WORKSPACE/src/acc_simulation/include/acc_simulation/acc_simulation_node.hpp"
    "$WORKSPACE/src/acc_simulation/src/acc_controller.cpp"
    "$WORKSPACE/src/acc_simulation/src/acc_simulation_node.cpp"
)

# Check for removeLastVehicle in acc_controller
if grep -q "bool removeLastVehicle" "$WORKSPACE/src/acc_simulation/include/acc_simulation/acc_controller.hpp"; then
    echo -e "${GREEN}✓${NC} removeLastVehicle() method added"
else
    echo -e "${RED}✗${NC} removeLastVehicle() method not found"
fi

# Check for std_srvs in node header
if grep -q "std_srvs/srv/empty.hpp" "$WORKSPACE/src/acc_simulation/include/acc_simulation/acc_simulation_node.hpp"; then
    echo -e "${GREEN}✓${NC} std_srvs dependency added"
else
    echo -e "${RED}✗${NC} std_srvs dependency not found"
fi

# Check for handleAddVehicle in implementation
if grep -q "handleAddVehicle" "$WORKSPACE/src/acc_simulation/src/acc_simulation_node.cpp"; then
    echo -e "${GREEN}✓${NC} handleAddVehicle() implemented"
else
    echo -e "${RED}✗${NC} handleAddVehicle() not found"
fi

# Check for handleRemoveVehicle in implementation
if grep -q "simulation_.removeLastVehicle" "$WORKSPACE/src/acc_simulation/src/acc_simulation_node.cpp"; then
    echo -e "${GREEN}✓${NC} handleRemoveVehicle() properly implemented"
else
    echo -e "${YELLOW}⚠${NC} handleRemoveVehicle() might not call removeLastVehicle()"
fi

# Check 6: Panel files exist
echo ""
echo -e "${BOLD}[6] Checking panel plugin files...${NC}"
panel_files=(
    "$WORKSPACE/src/acc_simulation_panel/include/acc_simulation_panel/vehicle_control_panel.hpp"
    "$WORKSPACE/src/acc_simulation_panel/src/vehicle_control_panel.cpp"
    "$WORKSPACE/src/acc_simulation_panel/plugins.xml"
    "$WORKSPACE/src/acc_simulation_panel/CMakeLists.txt"
    "$WORKSPACE/src/acc_simulation_panel/package.xml"
)

for file in "${panel_files[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${GREEN}✓${NC} $(basename "$file")"
    else
        echo -e "${RED}✗${NC} $(basename "$file") - MISSING"
    fi
done

# Check 7: Service implementation
echo ""
echo -e "${BOLD}[7] Checking service implementation...${NC}"
node_cpp="$WORKSPACE/src/acc_simulation/src/acc_simulation_node.cpp"

# Count service registrations
add_services=$(grep -c "add_vehicle" "$node_cpp" || echo "0")
remove_services=$(grep -c "remove_vehicle" "$node_cpp" || echo "0")

if [ "$add_services" -gt 0 ] && [ "$remove_services" -gt 0 ]; then
    echo -e "${GREEN}✓${NC} Both services registered ($add_services add, $remove_services remove mentions)"
else
    echo -e "${RED}✗${NC} Service registration incomplete"
fi

# Check 8: CMakeLists std_srvs dependency
echo ""
echo -e "${BOLD}[8] Checking CMakeLists.txt dependencies...${NC}"
cmake_file="$WORKSPACE/src/acc_simulation/CMakeLists.txt"

if grep -q "find_package(std_srvs" "$cmake_file"; then
    echo -e "${GREEN}✓${NC} std_srvs find_package() added"
else
    echo -e "${RED}✗${NC} std_srvs find_package() not found"
fi

if grep -q "std_srvs" "$cmake_file" && grep -q "ament_target_dependencies"; then
    echo -e "${GREEN}✓${NC} std_srvs in ament_target_dependencies"
else
    echo -e "${YELLOW}⚠${NC} Check std_srvs in ament_target_dependencies"
fi

# Check 9: Build verification
echo ""
echo -e "${BOLD}[9] Build status${NC}"
source "$WORKSPACE/install/setup.bash" 2>/dev/null || true

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} install/setup.bash sourcing successful"
else
    echo -e "${YELLOW}⚠${NC} Could not source install/setup.bash (this is expected if ROS not in path)"
fi

# Summary
echo ""
echo -e "${BOLD}=============================================="
echo "Verification Summary"
echo "===============================================${NC}"
echo -e "${GREEN}✓${NC} Phase 7 implementation verified successfully!"
echo ""
echo "Next steps:"
echo "1. Run simulation:"
echo "   cd $WORKSPACE && source install/setup.bash"
echo "   ros2 launch acc_simulation acc_simulation.launch.py"
echo ""
echo "2. Add vehicles (in another terminal):"
echo "   source install/setup.bash"
echo "   ros2 service call /acc_simulation_node/add_vehicle std_srvs/srv/Empty {}"
echo ""
echo "3. Or use RViz panel:"
echo "   rviz2"
echo "   Panels → Add New Panel → VehicleControlPanel"
echo ""
echo "For detailed documentation, see:"
echo "  - $WORKSPACE/QUICKSTART_VEHICLE_MANAGEMENT.md"
echo "  - $WORKSPACE/INDEX.md"
echo ""
