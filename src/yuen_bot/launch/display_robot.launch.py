import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory # 根据包的名称，返回路径

def generate_launch_description():
    # 获取默认路径
    urdf_tutorial_path = get_package_share_directory('yuen_bot')
    default_model_path = urdf_tutorial_path + '/urdf/fishbot/fishbot.urdf.xacro'
    default_rviz_path = urdf_tutorial_path + '/config/display_yuenbot_model.rviz'
    # 为了简化后续指令，这里声明了一个路径字符串，可以理解为类似环境变量的东西，这个路径字符串的名称为 'model'
    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
    name='model', default_value=str(default_model_path),
    description='URDF 的绝对路径 ')
    # 这里实现了一个用于构造 状态发布节点 robot_state_publisher 的参数
    subs_cmd_result = launch.substitutions.Command(['xacro ', launch.substitutions.LaunchConfiguration('model')])
    robot_description = launch_ros.descriptions.ParameterValue(subs_cmd_result, value_type=str)

    # 状态发布节点
    robot_state_publisher_node = launch_ros.actions.Node(
    package='robot_state_publisher',
    executable='robot_state_publisher',
    parameters=[{'robot_description': robot_description}] # 这是构造节点需要的参数，区别于 arguments
    )
    # 关节状态发布节点
    joint_state_publisher_node = launch_ros.actions.Node(
    package='joint_state_publisher',
    executable='joint_state_publisher',
    )
    # RViz 节点
    rviz_node = launch_ros.actions.Node(
    package='rviz2',
    executable='rviz2',
    # 下面运行此前手动调好的 rviz 配置文件
    arguments=['-d', default_rviz_path] # arguements 的作用是在终端的指令行当中加内容，区别于 parameters
    )

    return launch.LaunchDescription([
    action_declare_arg_mode_path,
    joint_state_publisher_node,
    robot_state_publisher_node,
    rviz_node
    ])