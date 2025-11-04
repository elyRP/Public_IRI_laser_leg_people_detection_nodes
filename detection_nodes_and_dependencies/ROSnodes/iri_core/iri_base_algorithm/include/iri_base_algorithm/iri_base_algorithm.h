// Copyright (C) 2010-2011 Institut de Robotica i Informatica Industrial, CSIC-UPC.
// Author Joan Perez
// All rights reserved.
//
// This file is part of iri-ros-pkg
// iri-ros-pkg is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _IRI_BASE_ALGORITHM_H
#define _IRI_BASE_ALGORITHM_H

#include <rclcpp/rclcpp.hpp> // old noetic: #include <ros/ros.h>
#include <signal.h>

// boost thread includes for ROS::spin thread
#include <thread>  //old ros noetic: #include <boost/thread.hpp>
#include <functional> // old ros noetic: #include <boost/bind.hpp>

// dynamic reconfigure server include
//#include <dynamic_reconfigure/server.h> // se mantiene igual.

// diagnostic updater include
#include <diagnostic_updater/diagnostic_updater.hpp> // old noetic: #include <diagnostic_updater/diagnostic_updater.h>

namespace algorithm_base
{

/**
 * \brief IRI ROS Algorithm Base Node Class
 *
 * This class provides a common framework for all kind of algorithms, similar to 
 * the one defined for the ROS driver_base::DriverNode<TemplateDriver> for the
 * driver environment. In this case, the template Algorithm class must be an
 * implementation derivated from a common interface (preferably ROS defined).
 * The inherit template design form allows complete access to the generic
 * algorithm object while manteining flexibility to instantiate any object which
 * inherits from it.
 *
 * An abstract class is generated as a ROS package to force implementation of
 * virtual methods in the generic algorithm layer. Functions to perform tests,
 * add diagnostics or dynamically reconfigure the algorithm parameters are
 * offered respecting the ROS driver_base terminology. Similarly to the
 * IriBaseNodeDriver common purpose behaviours for all algorithm kinds can be
 * defined and forwarded to each of the generic algorithm classes for specific
 * operations.
 *
 * In addition, a mainThread is provided like in the IriBaseNodeDriver class.
 * Threads are implemented using iri_utils software utilities. The mainThread() 
 * function loops in a defined loop_rate_. In each iteration, the abstract 
 * mainNodeThread() function defined in the inherit node class is called.
 */
template <class Algorithm>
class IriBaseAlgorithm : public rclcpp::Node
{
  public:
   /**
    * \brief config object
    *
    * All template Algorithm class will need a Config variable to allow ROS
    * dynamic reconfigure. This config class will contain all algorithm 
    * parameters which may be modified once the algorithm node is launched.
    */
    //typedef typename Algorithm::Config Config; // se mantiene igual, de momento.

  protected:
    /**
     * \brief Thread information structure
     *
     * This structure hold system level information of the thread and it is 
     * initialized when the thread is first created. This information can not 
     * be modified at any time and it is not accessible from outside the class.
     *
     */
    std::thread main_thread_; // old noetic: pthread_t thread;

   /**
    * \brief template algorithm class
    *
    * This template class refers to an implementation of an specific algorithm
    * interface. Will be used in the derivate class to define the common 
    * behaviour for all the different implementations from the same algorithm.
    */
    Algorithm alg_;  // se mantiene igual de momento.

   /**
    * \brief public node handle communication object
    *
    * This node handle is going to be used to create topics and services within
    * the node namespace. Additional node handles can be instantatied if 
    * additional namespaces are needed.
    */
    rclcpp::Node::SharedPtr public_node_handle_;// old ros onetic: ros::NodeHandle public_node_handle_;

   /**
    * \brief private node handle object
    *
    * This private node handle will be used to define algorithm parameters into
    * the ROS parametre server. For communication pruposes please use the 
    * previously defined node_handle_ object.
    */
    rclcpp::Node::SharedPtr private_node_handle_; // old noetic: ros::NodeHandle private_node_handle_;

   /**
    * \brief default main thread frequency
    * 
    * This constant determines the default frequency of the mainThread() in HZ.
    * All nodes will loop at this rate if loop_rate_ variable is not modified.
    */
    static constexpr unsigned int DEFAULT_RATE = 10; //[Hz]//old noetic: static const unsigned int DEFAULT_RATE = 10; //[Hz]
    
   /**
    * \brief diagnostic updater
    * 
    * The diagnostic updater allows definition of custom diagnostics. 
    * 
    */
    diagnostic_updater::Updater diagnostic_; // se mantiene igual de momento.

  public:
   /**
    * \brief constructor
    * 
    * This constructor initializes all the node handle objects, the main thread
    * loop_rate_ and the diagnostic updater. It also instantiates the main 
    * thread of the class and the dynamic reconfigure callback.
    */
    // old noetic: IriBaseAlgorithm(const ros::NodeHandle &nh = ros::NodeHandle("~"));
    IriBaseAlgorithm(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());
    /* otra traduccion posible en ros 2: 
    IriBaseAlgorithm(const std::string &node_name) :
      Node(node_name),
      loop_rate_(DEFAULT_RATE),
      diagnostic_(this)
    {
      RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::Constructor");

      // set the diagnostic updater period
      this->declare_parameter("diagnostic_period", 0.1);
    }*/

   /**
    * \brief destructor
    * 
    * This destructor kills the main thread.
    */
    // old ros noetic: ~IriBaseAlgorithm(void);
    ~IriBaseAlgorithm();
    /*{
      RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::Destructor");
      if (main_thread_.joinable())
      {
        main_thread_.join();
      }
    }*/
    
   /**
    * \brief spin
    * 
    * This method is meant to spin the node to update all ROS features. It also
    * launches de main thread. Once the object is instantiated, it will not 
    * start iterating until this method is called.
    */
    // old noetic: 
    int spin(void); // se quera igual, de momento.
    /*otra traduccion posible en ros2
        int spin(void)
    {
      RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::spin");

      // initialize diagnostics
      this->diagnostic_.setHardwareID("none");
      this->addDiagnostics();

      // create the status thread
      main_thread_ = std::thread(&IriBaseAlgorithm<Algorithm>::mainThread, this);

      rclcpp::Rate rate(loop_rate_);
      while (rclcpp::ok())
      {
        rclcpp::spin_some(this->get_node_base_interface());
        this->diagnostic_.force_update();
        rate.sleep();
      }

      return 0;
    }*/
    

    int nodelet_spin(void); // TODO: no equivalencia, de momento.

  private:
   /**
    * \brief ros spin thread
    * 
    * This boost thread object will manage the ros::spin function. It is 
    * instantiated and launched in the spin class method.
    */
    std::shared_ptr<std::thread> ros_thread_; //boost::shared_ptr<boost::thread> ros_thread_; // old ros noetic

   /**
    * \brief dynamic reconfigure server
    * 
    * The dynamic reconfigure server is in charge to retrieve the parameters
    * defined in the config cfg file through the reconfigureCallback.
    */
    //dynamic_reconfigure::Server<Config> dsrv_; // se queda igual de momento. dynamic reconfigure no es equivalente en ros2

   /**
    * \brief main thread loop rate
    * 
    * This value determines the loop frequency of the node main thread function
    * mainThread() in HZ. It is initialized at construction time. This variable 
    * may be modified in the node implementation constructor if a desired
    * frequency is required.
    */
     rclcpp::Rate loop_rate_;// old noetic: ros::Rate loop_rate_;

  protected:

    /**
    * \brief 
    * 
    */
    void setRate(double rate_hz); //igual de momento.


    /**
    * \brief 
    * 
    */
    double getRate(void); //igual de momento.

   /**
    * \brief dynamic reconfigure server callback
    * 
    * This method is called whenever a new configuration is received through
    * the dynamic reconfigure. The derivated generic algorithm class must 
    * implement it.
    *
    * \param config an object with new configuration from all algorithm 
    *               parameters defined in the config file.
    * \param level  integer referring the level in which the configuration
    *               has been changed.
    */
    //void reconfigureCallback(Config &config, uint32_t level); //igual de momento.

   /**
    * \brief dynamic reconfigure server callback
    * 
    * This method is called whenever a new configuration is received through
    * the dynamic reconfigure. The derivated generic algorithm class must 
    * implement it.
    *
    * \param config an object with new configuration from all algorithm 
    *               parameters defined in the config file.
    * \param level  integer referring the level in which the configuration
    *               has been changed.
    */
    //virtual void node_config_update(Config &config, uint32_t level) = 0; //igual de momento.
   
   /**
    * \brief add diagnostics
    * 
    * In this function ROS diagnostics applied to all algorithms nodes may be
    * added. It calls the addNodeDiagnostics method.
    */
    void addDiagnostics(void); //igual de momento.

   /**
    * \brief node add diagnostics
    *
    * In this abstract function additional ROS diagnostics applied to the 
    * specific algorithms may be added.
    */
    virtual void addNodeDiagnostics(void) = 0; //igual de momento.

   /**
    * \brief main node thread
    *
    * This is the main thread node function. Code written here will be executed
    * in every inherit algorithm node object. The loop won't exit until the node
    * finish its execution. The commands implemented in the abstract function
    * mainNodeThread() will be executed in every iteration.
    * 
    * Loop frequency can be tuned my modifying loop_rate_ attribute.
    * 
    * \param param is a pointer to a IriBaseAlgorithm object class. It is used
    *              to access to the object attributes and methods.
    */
   void *mainThread(void *param); // igual de momento.

   /**
    * \brief specific node thread
    *
    * In this abstract function specific commands for each algorithm node
    * have to be detailed.
    */
    virtual void mainNodeThread(void) = 0; //igual de momento.

    static void hupCalled(int sig); // igual de momento.
};

/* old noetic:template <class Algorithm>
IriBaseAlgorithm<Algorithm>::IriBaseAlgorithm(const ros::NodeHandle &nh) : 
  public_node_handle_(nh),
  private_node_handle_("~"), 
  loop_rate_(DEFAULT_RATE),
  diagnostic_(),
  dsrv_(public_node_handle_)
 {*/
template <class Algorithm>
IriBaseAlgorithm<Algorithm>::IriBaseAlgorithm(const rclcpp::NodeOptions & options) :
  rclcpp::Node("iri_base_algorithm", options),
  public_node_handle_(this->shared_from_this()),
  private_node_handle_(this->shared_from_this()),
  loop_rate_(DEFAULT_RATE),
  diagnostic_(this)
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::Constructor");// old noeticROS_DEBUG("IriBaseAlgorithm::Constructor");

  // allow Ctrl+C management
  signal(SIGHUP, &IriBaseAlgorithm<Algorithm>::hupCalled); // de momento igual.

  // set the diagnostic updater period
  this->declare_parameter("diagnostic_period", 0.1);// old noetic: this->private_node_handle_.setParam("diagnostic_period",0.1);
}



template <class Algorithm>
IriBaseAlgorithm<Algorithm>::~IriBaseAlgorithm()
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::Destructor");
  if (main_thread_.joinable())
  {
    main_thread_.join();
  }
}

/*old noetic:
template <class Algorithm>
IriBaseAlgorithm<Algorithm>::~IriBaseAlgorithm()
{
  ROS_DEBUG("IriBaseAlgorithm::Destructor");
  pthread_cancel(this->thread);
  pthread_join(this->thread,NULL);
}*/

/* old noetic:
template <class Algorithm>
void IriBaseAlgorithm<Algorithm>::setRate(double rate_hz)
{
  this->loop_rate_=ros::Rate(rate_hz);
}*/
template <class Algorithm>
void IriBaseAlgorithm<Algorithm>::setRate(double rate_hz)
{
  //this->loop_rate_ = rclcpp::Rate(rate_hz);
  rclcpp::Rate loop_rate_{rate_hz};
}

/*old noetic:
template <class Algorithm>
double IriBaseAlgorithm<Algorithm>::getRate(void)
{
  return 1.0/this->loop_rate_.expectedCycleTime().toSec();
}*/

template <class Algorithm>
double IriBaseAlgorithm<Algorithm>::getRate(void)
{
   auto period = this->loop_rate_.period();
   return period.count() * 1e-9;;
  //old noetic: return 1.0 / this->loop_rate_.cycle_time().count();
}

/*old noetic:
template <class Algorithm>
void IriBaseAlgorithm<Algorithm>::reconfigureCallback(Config &config, uint32_t level)
{
  ROS_DEBUG("IriBaseAlgorithm::reconfigureCallback");
  this->node_config_update(config, level);
  this->alg_.config_update(config, level);
}*/

//template <class Algorithm>
/*void IriBaseAlgorithm<Algorithm>::reconfigureCallback(Config &config, uint32_t level)
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::reconfigureCallback");
  this->node_config_update(config, level);
  this->alg_.config_update(config, level);
}*/

/*old noetic:
template <class Algorithm>
void IriBaseAlgorithm<Algorithm>::addDiagnostics(void)
{
  ROS_DEBUG("IriBaseAlgorithm::addDiagnostics");
  addNodeDiagnostics();
}*/

template <class Algorithm>
void IriBaseAlgorithm<Algorithm>::addDiagnostics(void)
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::addDiagnostics");
  addNodeDiagnostics();
}

/*old noetic:
template <class Algorithm>
void *IriBaseAlgorithm<Algorithm>::mainThread(void *param)
{
  ROS_DEBUG("IriBaseAlgorithm::mainThread");

  // retrieve base algorithm class
  IriBaseAlgorithm *iriNode = (IriBaseAlgorithm *)param;

  while(ros::ok())
  {
    // run node stuff
    iriNode->mainNodeThread();

    // sleep remainder time
    iriNode->loop_rate_.sleep();
  }

  // kill main thread
  pthread_exit(NULL);
}*/
template <class Algorithm>
void *IriBaseAlgorithm<Algorithm>::mainThread(void *param)
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::mainThread");

  while(rclcpp::ok())
  {
    this->mainNodeThread();
    this->loop_rate_.sleep();
  }
}

template <class Algorithm>
void IriBaseAlgorithm<Algorithm>::hupCalled(int sig)
{
  RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "Unexpected SIGHUP caught. Ignoring it.");// old noetic:ROS_WARN("Unexpected SIGHUP caught. Ignoring it.");
}

template <class Algorithm>
int IriBaseAlgorithm<Algorithm>::spin(void)
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::spin");// old noetic: ROS_DEBUG("IriBaseAlgorithm::spin");

  // initialize diagnostics
  this->diagnostic_.setHardwareID("none");
  this->addDiagnostics();
  
  // launch ros spin in different thread
  this->ros_thread_ = std::make_unique<std::thread>(&IriBaseAlgorithm::spin, this);
  // this->ros_thread_ = std::make_unique<std::thread>(&rclcpp::spin);// old noetic: this->ros_thread_.reset( new boost::thread(boost::bind(&ros::spin)) );
  assert(ros_thread_ && ros_thread_->joinable());// old noetic:assert(ros_thread_);

  // assign callback to dynamic reconfigure server
  //this->dsrv_.setCallback(boost::bind(&IriBaseAlgorithm<Algorithm>::reconfigureCallback, this, _1, _2)); //igual, no hay queivalente.

  // create the status thread
  //this->thread = std::thread(&IriBaseAlgorithm::mainThread, this);
   // otra traduccion a ros2 humble: this->thread=std::make_unique<std::thread>(this->mainThread, this);// old noetic: pthread_create(&this->thread,NULL,this->mainThread,this);
  
  
  /* old noetic:
  while(ros::ok())
  {
    // update diagnostics
    this->diagnostic_.update();
    
    ros::WallDuration(this->diagnostic_.getPeriod()).sleep();
  }*/
  while(rclcpp::ok())
  {
    this->diagnostic_.force_update();
    
    rclcpp::sleep_for(this->diagnostic_.getPeriod().to_chrono<std::chrono::nanoseconds>()); 
  }

  // stop ros
  rclcpp::shutdown();// old noetic: ros::shutdown();

  // kill ros spin thread
  // old noetic: this->ros_thread_.reset();
  if (main_thread_.joinable())
  {
    main_thread_.join();
  }

  return 0;
}

template <class Algorithm>
int IriBaseAlgorithm<Algorithm>::nodelet_spin(void)
{
  RCLCPP_DEBUG(this->get_logger(), "IriBaseAlgorithm::spin"); // old ros noetic: ROS_DEBUG("IriBaseAlgorithm::spin");

  // initialize diagnostics
  this->diagnostic_.setHardwareID("none"); //remain the same
  this->addDiagnostics();
  
  // assign callback to dynamic reconfigure server
  //this->dsrv_.setCallback(boost::bind(&IriBaseAlgorithm<Algorithm>::reconfigureCallback, this, _1, _2)); //seems remain the same

  // create the status thread
  this->thread_ = std::thread(&IriBaseAlgorithm::mainThread, this);// old noetic: pthread_create(&this->thread,NULL,this->mainThread,this);

  /*old noetic:
   while(ros::ok())
  {
    // update diagnostics
    this->diagnostic_.update();
    
    ros::WallDuration(this->diagnostic_.getPeriod()).sleep();
  }*/
  while(rclcpp::ok())
  {
    this->diagnostic_.force_update();
    rclcpp::sleep_for(this->diagnostic_.getPeriod().to_chrono<std::chrono::nanoseconds>());//rclcpp::sleep_for(std::chrono::duration_cast<std::chrono::nanoseconds>(this->diagnostic_.getPeriod()));
  }

  // stop ros
  rclcpp::shutdown(); // old noetic: ros::shutdown();
  
  /* ros2 tambien sugeria esto, pero aqui el codigo original no reinicia el thread:
 if (main_thread_.joinable())
  {
    main_thread_.join();
  }
  */

  return 0;
}

// definition of the static Ctrl+C counter

/**
 * \brief base main
 *
 * This main is common for all the algorithm nodes. The AlgImplTempl class
 * refers to an specific implementation derived from a generic algorithm.
 * 
 * First, ros::init is called providing the node name. Ctrl+C control is 
 * activated for sercure and safe exit. Finally, an AlgImplTempl object is
 * defined and launched to spin for endless loop execution.
 * 
 * \param argc integer with number of input parameters
 * \param argv array with all input strings
 * \param node_name name of the node
 */
template <class AlgImplTempl>
int main(int argc, char **argv, std::string node_name) // ros 2 me lo sugiere cambiado, pero creo que no: int main(int argc, char **argv, const std::string & node_name)
{
   
  RCLCPP_DEBUG(rclcpp::get_logger("rclcpp"), "IriBaseAlgorithm::%s Launched", node_name.c_str());
  // old noetic: ROS_DEBUG("IriBaseAlgorithm::%s Launched", node_name.c_str());

  // ROS initialization
  rclcpp::init(argc, argv);// old noetic: ros::init(argc, argv, node_name);

  // define and launch generic algorithm implementation object
  auto algImpl = std::make_shared<AlgImplTempl>(); // old noetic: AlgImplTempl algImpl;
  algImpl->spin(); // old noetic: algImpl.spin();

   // ros 2 sugiere esto, pero creo que no, al no tenerlo el codigo original: rclcpp::shutdown();
  return 0;
}

}
#endif
