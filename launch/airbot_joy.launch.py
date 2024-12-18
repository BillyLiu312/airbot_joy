# Copyright 2021 Stogl Robotics Consulting UG (haftungsbeschränkt)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
from launch import LaunchDescription
from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration

import sys
sys.path.insert(0, os.path.join(get_package_share_directory('tita_bringup'), 'launch'))
from launch_utils import tita_namespace
prefix=tita_namespace

def generate_launch_description():
    # Declare an argument to control inclusion of the extra launch file
    declared_arguments = []
    declared_arguments.append(
        DeclareLaunchArgument(
            "log_level",
            default_value='DEBUG',
            description="Decide use tita tower or not",
        )
    )

    airbot_joy = Node(
        package='airbot_joy',
        executable='airbot_joy_node',
        output='screen',
        parameters=[os.path.join(get_package_share_directory('airbot_joy'), 'config', 'param.yaml'),
                    {'log_level': LaunchConfiguration('log_level')}],
        namespace=prefix,
    )
    
    return LaunchDescription(declared_arguments + [
        airbot_joy,
    ])
