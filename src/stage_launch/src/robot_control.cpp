#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"
#include <ctime>

bool obstacle = false;
bool obstacle2 = false;
bool obstacle3 = false;
bool obstacle4 = false;
double start;
float data=0;
ros::Publisher pub;
bool stage1=1;
bool stage2=0;
bool stage3=0;
bool stage4=0;
bool stage5=0;

void laserCallback(const sensor_msgs::LaserScan& msg)
{
  ROS_DEBUG_STREAM("Laser msg: "<<msg.scan_time);

  const double kMinRange = 8;
  //проверим нет ли вблизи робота препятствия
  data=0;
for(int d=175;d<180;d++)
{
    data=data+msg.ranges[d];
}
      if (msg.ranges[90] < kMinRange)
	  {
		  obstacle = true;
                  ROS_WARN_STREAM("OBSTACLE1");

	  }
      for(int i=85;i<95;i++)
      {
      if (msg.ranges[i] < 1)
          {
                  obstacle2 = true;
                  ROS_WARN_STREAM("OBSTACLE2");

          }
}
      if (msg.ranges[179] < 1)
          {
                  obstacle3 = true;
                  ROS_WARN_STREAM(msg.ranges[170]);

          }
      if ((msg.ranges[179] < 3)&&((msg.ranges[1] < 3)))
          {
                  obstacle4 = true;
                  ROS_WARN_STREAM(4);

          }
}




void poseCallback(const nav_msgs::Odometry& msg)
{
  ROS_DEBUG_STREAM("Pose msg: x = " << msg.pose.pose.position.x<<
          " y = " << msg.pose.pose.position.y <<
          " theta = " << 2*atan2(msg.pose.pose.orientation.z,
                  msg.pose.pose.orientation.w) );
}

void timerCallback(const ros::TimerEvent&)
{
	static int counter = 0;
	counter++;

	ROS_DEBUG_STREAM("on timer "<<counter);

	geometry_msgs::Twist cmd;
        stage4=0;
        if(stage2==0)
        {
        if(!obstacle)
        {cmd.angular.z = -2;stage4=0;stage3=0;}
        else
        {stage2=1;}
        };
        if((stage2==1)&&(!obstacle2))
        {
        cmd.linear.x=5;
        stage3=0;
        stage4=0;
        }else
        {stage3=1;stage2=0;}
        if((stage3==1)&&(!obstacle3))
        {
            stage4=0;
        cmd.angular.z=-1;

        }else
        {stage4=1;stage3=0;}
        if((stage4==1)&&(!obstacle4)&&(stage3==0)&&(stage2==0))
        {
        cmd.angular.z=3*((data/5)-1);
        cmd.linear.x=0.8;
        }else
        {stage4=0;}
        if((obstacle4)&&(stage4==0))
        {
            if(start==0)
            {
                start=clock();
            }
             ROS_WARN_STREAM(start);
            if(clock()-start<300000)
            {cmd.linear.x=1;
        cmd.angular.z=0;}
            else{
                cmd.linear.x=0;
            }
        }

        pub.publish(cmd);
}

int main(int argc, char **argv)
{
  /**
   * Инициализация системы сообщений ros
   * Регистрация node с определенным именем (третий аргумент функции)
   * Эта функция должна быть вызвана в первую очередь
   */
  ros::init(argc, argv, "control_node");

  /**
   * NodeHandle  - объект через который осуществляется взаимодействие с ROS:
   * передача сообщений
   * регистрация коллбаков (функций обработки сообщений)
   */
  ros::NodeHandle n;

  /**
   * subscribe() функция подписки на сообщения определенного типа,
   * передаваемое по заданному топику
   * В качестве параметров указываются
   * - топик - на сообщения которого происходит подписка
   * - длина очереди сообщений хранящихся до обработки (если очередь заполняется,
   *  то самые старые сообщения будут автоматически удаляться )
   *  - функция обработки сообщений
   *
   *
   *  Подписываемся на данные дальномера

   */
  ros::Subscriber laser_sub = n.subscribe("base_scan", 1, laserCallback);

  /*
   * Подписываемся на данные о положении робота
   */
  ros::Subscriber pose_sub = n.subscribe("base_pose_ground_truth", 1, poseCallback);


  /*
   * Регистрируем функцию обработчик таймера 10Hz
   */
  ros::Timer timer1 = n.createTimer(ros::Duration(0.1), timerCallback);

  /*
   * Сообщаем, что мы будем публиковать сообщения типа Twist по топику cmd_vel
   * второй параметр - длина очереди
   */
  pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);

   /**
   * ros::spin() функция внутри которой происходит вся работа по приему сообщений
   * и вызову соответствующих обработчиков . Вся обработка происходит из основного потока
   * (того, который вызвал ros::spin())
   * Функция будет завершена, когда подьзователь прервет выполнение процесса с Ctrl-C
   *
   */
  ros::spin();

  return 0;
}
