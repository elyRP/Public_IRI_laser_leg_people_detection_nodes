import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.descriptions import ParameterFile
from nav2_common.launch import RewrittenYaml
from launch.actions import SetEnvironmentVariable
from launch_ros.actions import LifecycleNode  # es por el map server
 #from launch_ros.actions import LifecycleTransition  # es por el map server
import rclpy  # es por el map server
from rclpy.node import Node as RclpyNode  # es por el map server
from lifecycle_msgs.srv import ChangeState  # es por el map server
from lifecycle_msgs.msg import Transition   # es por el map server
from launch.actions import ExecuteProcess, TimerAction  # es por el map server

def generate_launch_description():
    # Environment
    package_dir = get_package_share_directory('iri_laser_people_detection')
    
    package_dir_filter = get_package_share_directory('iri_laser_people_map_filter')
    
    package_dir_trackeing = get_package_share_directory('iri_people_tracking_mht')
    
    package_dir_planner = get_package_share_directory('iri_robot_aspsi')
    
    # Constant parameters
    lifecycle_nodes = ['collision_monitor']
    autostart = True

    #set_target_properties(iri_laser_people_detection PROPERTIES INSTALL_RPATH "/usr/local/lib")
    SetEnvironmentVariable('LD_LIBRARY_PATH', '/usr/local/lib/iri/iriutils/:' + os.environ.get('$LD_LIBRARY_PATH', ''))
    SetEnvironmentVariable('LD_LIBRARY_PATH', '/usr/local/lib/iri/laser_people_detection/:' + os.environ.get('$LD_LIBRARY_PATH', ''))
    SetEnvironmentVariable('LD_LIBRARY_PATH', '/usr/local/lib/iri/people_tracking_mht/:' + os.environ.get('$LD_LIBRARY_PATH', ''))
    SetEnvironmentVariable('LD_LIBRARY_PATH', '/usr/local/lib/iridrivers/:' + os.environ.get('$LD_LIBRARY_PATH', ''))
    # Launch arguments
    # 1. Create the launch configuration variables
    ns = LaunchConfiguration('ns')
    config_file_detector = LaunchConfiguration('config_file_detector')
    config_file_filter = LaunchConfiguration('config_file_filter')
    config_file_tracker = LaunchConfiguration('config_file_tracker')
    config_file_planner = LaunchConfiguration('config_file_planner')
     
    scan_topic = LaunchConfiguration('scan_topic')
    people_topic = LaunchConfiguration('people_topic')
    use_sim_time = LaunchConfiguration('use_sim_time')
    
    DeclareLaunchArgument('scan_topic', default_value='/scan')
    
    DeclareLaunchArgument('people_topic', default_value='/lpd/people')
    
    DeclareLaunchArgument('markers_topic', default_value='/lpd/markers')
    
                
    # 2. Declare the launch arguments
    declare_use_sim_time_cmd = DeclareLaunchArgument(
        'use_sim_time',
        default_value='True',
        description='Use simulation (Gazebo) clock if true')
        
    declare_ns_cmd = DeclareLaunchArgument(
        'ns',
        default_value='tibi',
        description='robot namespace')

    
    declare_config_file_detector_cmd = DeclareLaunchArgument(
         'config_file_detector',
         default_value=os.path.join(package_dir, 'config', 'params_forNav.yaml'),
         description='Full path to the ROS2 parameters file to use for all launched detection node')
         
    declare_config_file_filter_cmd = DeclareLaunchArgument(
         'config_file_filter',
         default_value=os.path.join(package_dir_filter, 'config', 'params.yaml'),
         description='Full path to the ROS2 parameters file to use for all launched filter node')
       
    declare_config_file_tracker_cmd = DeclareLaunchArgument(
         'config_file_tracker',
         default_value=os.path.join(package_dir_filter, 'config', 'params.yaml'),
         description='Full path to the ROS2 parameters file to use for all launched tracker node')
         
    declare_config_file_planner_cmd = DeclareLaunchArgument(
         'config_file_planner',
         default_value=os.path.join(package_dir_planner, 'config/turtlebot3_burgerHumble2025', 'akp_local_planner_params.yaml'),
         description='Full path to the ROS2 parameters file to use for all launched local planner node')
        
    declare_scan_topic_cmd = DeclareLaunchArgument(
        'scan_topic',
        default_value='/scan',
        description=' scan topic name')
        
    declare_people_topic_cmd = DeclareLaunchArgument(
        'people_topic',
        default_value='/people',
        description=' people topic name')        

    # Create our own temporary YAML files that include substitutions
    param_substitutions = {
        'use_sim_time': use_sim_time}

    #  configured_params = ParameterFile(
     #     RewrittenYaml(
     #         source_file=params_file,
      #        root_key=namespace,
      #        param_rewrites=param_substitutions,
       #       convert_types=True),
       #   allow_substs=True)

    # Nodes launching commands
    start_iri_laser_people_detection_cmd = Node(
            package='iri_laser_people_detection',
            executable='iri_laser_people_detection',
            name='lpd',
            output='screen',
            emulate_tty=True,  # https://github.com/ros2/launch/issues/188
            parameters=[config_file_detector],
            #remappings=[
            #    ('scan', '/scan'),
            #    ('people', '/people_topic'),
            #    ('markers', '/markers_topic'),
            #]
            
            )
             #parameters=[{'use_sim_time': use_sim_time,
             #           'ns': ns,
             #           'scan_topic': scan_topic,
             #           'people_topic': people_topic,
             #           'config_file': config_file}])


    start_iri_laser_people_map_filter_cmd = Node(
            package='iri_laser_people_map_filter',
            executable='iri_laser_people_map_filter',
            name='lpdfilter',
            output='screen',
            emulate_tty=True,  # https://github.com/ros2/launch/issues/188
            parameters=[config_file_filter],
            #remappings=[
            #    ('scan', '/scan'),
            #    ('people', '/people_topic'),
            #    ('markers', '/markers_topic'),
            #]
            
            )
            
    start_iri_laser_people_map_tracker_cmd = Node(
            package='iri_people_tracking_mht',
            executable='iri_people_tracking_mht',
            name='mht',
            output='screen',
            emulate_tty=True,  # https://github.com/ros2/launch/issues/188
            parameters=[config_file_tracker],
            remappings=[
                ('/detections', '/people_filtered_out'),
                ('/odom_for_medium_velocity', '/odom'),
            ]
            #remappings=[
            #    ('scan', '/scan'),
            #    ('people', '/people_topic'),
            #    ('markers', '/markers_topic'),
            #]
            
            )
                       
                
    start_static_tf_map_to_odom_cmd = Node(
            package='my_tf_map_create',  # Reemplaza con el nombre real del paquete
            executable='static_tf_map_to_odom',  # Nombre del ejecutable compilado
            name='static_tf_node',
            output='screen'
        )
    
    start_map_server_cmd = LifecycleNode(
            package='nav2_map_server',
            executable='map_server',
            name='map_server',
            namespace='',  # <--- ¡esto es lo que faltaba!
            output='screen',
            #emulate_tty=True, 
            parameters=[{'use_sim_time': True}, {'yaml_filename': '/home/elyupc2204msi/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_map_filter/maps/rooms.yaml'}]
            #parameters=['/home/ely/ros2_ws/src/codeSubjectMobileRobots/plannerSRC/people_detection_nodes-Detector_nodes_in_ROS2_humble/iri_laser_people_map_filter/maps/rooms.yaml']
            )
            
            
    start_iri_robot_aspsi_cmd = Node(
            package='iri_robot_aspsi',
            executable='libiri_robot_aspsi',
            name='aspsi',
            output='screen',
            emulate_tty=True,  # https://github.com/ros2/launch/issues/188
            parameters=[config_file_planner],
            remappings=[
                ('/detections', '/people_filtered_out'),
                ('/odom_for_medium_velocity', '/odom'),
            ]
            #remappings=[
            #    ('scan', '/scan'),
            #    ('people', '/people_topic'),
            #    ('markers', '/markers_topic'),
            #]
            
            )
    
    configure = ExecuteProcess(
        cmd=['ros2', 'lifecycle', 'set', '/map_server', 'configure'],
        shell=True
    )

    activate = ExecuteProcess(
        cmd=['ros2', 'lifecycle', 'set', '/map_server', 'activate'],
        shell=True,
        output='screen'
    )


    # Ejecutar las transiciones con delays
    configure_after_start = TimerAction(period=2.0, actions=[configure])
    activate_after_configure = TimerAction(period=4.0, actions=[activate])

    ld = LaunchDescription([
        start_map_server_cmd,
        TimerAction(period=2.0, actions=[configure]),
        TimerAction(period=4.0, actions=[activate]),
    ])

    # Launch arguments
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(declare_ns_cmd)
    ld.add_action(declare_config_file_detector_cmd)
    ld.add_action(declare_config_file_filter_cmd)
    ld.add_action(declare_config_file_tracker_cmd)
    ld.add_action(declare_config_file_planner_cmd)
    ld.add_action(declare_scan_topic_cmd)
    ld.add_action(declare_people_topic_cmd)
    
    # Node launching commands
    ld.add_action(start_iri_laser_people_detection_cmd)
    ld.add_action(start_iri_laser_people_map_filter_cmd)
    ld.add_action(start_iri_laser_people_map_tracker_cmd)
    ld.add_action(start_static_tf_map_to_odom_cmd)
    ld.add_action(start_iri_robot_aspsi_cmd)
     # ld.add_action(start_map_server_cmd)

    return ld
