from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='cpp_parameters',
            executable='param_node',
            name='custom_cpp_parameters_node',
            output='screen',
            emulate_tty=True,
            parameters=[
                {'my_parameter': 'test_value_launch_file'}
            ]
        )
    ])