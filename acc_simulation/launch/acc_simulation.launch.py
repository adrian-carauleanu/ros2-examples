#!/usr/bin/env python3
"""
Launch file for ACC Simulation node
"""

import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Declare launch arguments
    desired_speed_arg = DeclareLaunchArgument(
        'desired_speed',
        default_value='20.0',
        description='Desired speed in m/s'
    )

    desired_distance_arg = DeclareLaunchArgument(
        'desired_distance',
        default_value='30.0',
        description='Desired distance to lead vehicle in meters'
    )

    kp_arg = DeclareLaunchArgument(
        'kp',
        default_value='0.5',
        description='Proportional gain for distance control'
    )

    kv_arg = DeclareLaunchArgument(
        'kv',
        default_value='0.3',
        description='Proportional gain for speed control'
    )

    update_frequency_arg = DeclareLaunchArgument(
        'update_frequency',
        default_value='20.0',
        description='Update frequency in Hz'
    )

    # Create node
    acc_sim_node = Node(
        package='acc_simulation',
        executable='acc_simulation_node',
        name='acc_simulation',
        output='screen',
        parameters=[
            {'desired_speed': LaunchConfiguration('desired_speed')},
            {'desired_distance': LaunchConfiguration('desired_distance')},
            {'kp': LaunchConfiguration('kp')},
            {'kv': LaunchConfiguration('kv')},
            {'update_frequency': LaunchConfiguration('update_frequency')},
        ]
    )

    # Create RViz node
    rviz_config_path = os.path.join(
        get_package_share_directory('acc_simulation'),
        'launch',
        'acc_simulation.rviz'
    )
    
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config_path],
    )

    return LaunchDescription([
        desired_speed_arg,
        desired_distance_arg,
        kp_arg,
        kv_arg,
        update_frequency_arg,
        acc_sim_node,
        rviz_node,
    ])
