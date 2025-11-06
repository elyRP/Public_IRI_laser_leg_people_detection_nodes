import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.descriptions import ParameterFile
from nav2_common.launch import RewrittenYaml
from launch.actions import SetEnvironmentVariable

def generate_launch_description():
    # Environment
    package_dir = get_package_share_directory('iri_laser_people_detection')
    
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



    ld = LaunchDescription()

    # Launch arguments
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(declare_ns_cmd)
    ld.add_action(declare_config_file_detector_cmd)
    ld.add_action(declare_scan_topic_cmd)
    ld.add_action(declare_people_topic_cmd)
    
    # Node launching commands
    ld.add_action(start_iri_laser_people_detection_cmd)

    return ld
