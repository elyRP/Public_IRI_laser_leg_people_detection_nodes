# Copyright 2023 Clearpath Robotics, Inc.
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
#
# @author Roni Kreinin (rkreinin@clearpathrobotics.com)

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import IncludeLaunchDescription
from launch.actions import SetEnvironmentVariable
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
import launch_ros.actions

ARGUMENTS = [
    DeclareLaunchArgument('use_sim_time', default_value='true',
                          choices=['true', 'false'],
                          description='use_sim_time'),
    DeclareLaunchArgument('namespace', default_value='',
                          description='Robot namespace'),
    DeclareLaunchArgument('world', default_value='empty',
                          description='Ignition World'),
    DeclareLaunchArgument('model', default_value='lite',
                          choices=['standard', 'lite'],
                          description='Turtlebot4 Model'),
    DeclareLaunchArgument('oak', default_value='false',
                          choices=['true', 'false'],
                          description='OAK Camera enabled'),
]

for pose_element in ['x', 'y', 'z', 'yaw']:
    ARGUMENTS.append(DeclareLaunchArgument(pose_element, default_value='0.0',
                     description=f'{pose_element} component of the robot pose.'))


def generate_launch_description():

    # set params safety_override and reflexes
    safety_param_set = launch_ros.actions.SetParameter(name='safety_override', value="full")
    reflexes_param_set = launch_ros.actions.SetParameter(name='reflexes_enabled', value=False)

    # Software instead of GPU
    set_opengl = SetEnvironmentVariable(name='LIBGL_ALWAYS_SOFTWARE', value=['True'])
    
    # Directories
    #pkg_upc_mrn = get_package_share_directory('upc_mrn')
    # Directories
    pkg_iri_laser_people_detection = get_package_share_directory('iri_laser_people_detection')


    # Paths
    #ignition_launch = PathJoinSubstitution(
    #    [pkg_upc_mrn, 'launch', 'ignition.launch.py'])
    #robot_spawn_launch = PathJoinSubstitution(
    #    [pkg_upc_mrn, 'launch', 'turtlebot4_spawn.launch.py'])
    # Paths
    #ignition_launch = PathJoinSubstitution(
    #    [pkg_iri_laser_people_detection, 'launch', 'ignition.launch.py'])
    robot_spawn_launch = PathJoinSubstitution(
        [pkg_iri_laser_people_detection, 'launch', 'turtlebot4_spawn.launch.py'])

    #ignition = IncludeLaunchDescription(
    #    PythonLaunchDescriptionSource([ignition_launch]),
    #    launch_arguments=[
    #        ('world', LaunchConfiguration('world'))
    #    ]
    #)

    robot_spawn = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([robot_spawn_launch]),
        launch_arguments=[
            ('namespace', LaunchConfiguration('namespace')),
            ('x', LaunchConfiguration('x')),
            ('y', LaunchConfiguration('y')),
            ('z', LaunchConfiguration('z')),
            ('yaw', LaunchConfiguration('yaw')),
            ('oak', LaunchConfiguration('oak'))]
    )

    # Create launch description and add actions
    ld = LaunchDescription(ARGUMENTS)
    ld.add_action(safety_param_set)
    ld.add_action(reflexes_param_set)
    ld.add_action(set_opengl)
    #ld.add_action(ignition)
    ld.add_action(robot_spawn)
    return ld
