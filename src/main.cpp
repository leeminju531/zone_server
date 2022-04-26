
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <boost/filesystem.hpp>
#include <unordered_map>

#include "ros/ros.h"
#include "ros/console.h"
#include "map_server/image_loader.h"
#include "nav_msgs/MapMetaData.h"
#include "nav_msgs/LoadMap.h"
#include "std_msgs/String.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <tf/tf.h>
#include <tf2_ros/transform_listener.h>


#include <tf2_geometry_msgs/tf2_geometry_msgs.h>


#include <tf/LinearMath/Transform.h>


#include <tf2/LinearMath/Transform.h>


#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
// #include <tf/transform.h>
#include <tf/transform_broadcaster.h> //temp

#include <unistd.h>// time 
using namespace std;

#ifdef HAVE_YAMLCPP_GT_0_5_0
// The >> operator disappeared in yaml-cpp 0.5, so this function is
// added to provide support for code written under the yaml-cpp 0.3 API.
template<typename T>
void operator >> (const YAML::Node& node, T& i)
{
  i = node.as<T>();
}
#endif


enum Color {
  COLOR_A = 10,
  COLOR_B = 30,
  COLOR_C = 50,
  COLOR_D = 70,
  COLOR_E = 90,
  COLOR_F = 110,
};
unsigned char color[6] = {COLOR_A,COLOR_B,COLOR_C,COLOR_D,COLOR_E,COLOR_F};

class RobotZoneServer{
public:
    RobotZoneServer():
        private_nh_("~")
        
    {
        private_nh_.param("map_metadata", map_metadata_, std::string("map_metadata"));
        private_nh_.param("global_frame_id", global_frame_id_, std::string("map"));
        private_nh_.param("base_frame_id", base_frame_id_, std::string("base_footprint"));
        private_nh_.param("zone_folder_path",zone_folder_path_,std::string(""));
        private_nh_.param("pub_each_map",each_pub_,bool(true));
        
        
        robot_zone_pub_ = nh_.advertise<std_msgs::String> ("robot_zone",1);
        map_metadata_sub_ = nh_.subscribe(map_metadata_,1,&RobotZoneServer::map_callback,this);
        zone_meta_pub_ = nh_.advertise<nav_msgs::MapMetaData>("zone_metadata",1,true);
        zone_map_pub_ = nh_.advertise<nav_msgs::OccupancyGrid>("zone_cost_map",1,true);
        boost::thread(boost::bind(&RobotZoneServer::WorkerThread,this));
        
        // robotZonePublish();
    }
    ~RobotZoneServer()
    {
      
      zone_thr_.join();
    }
private:
    ros::NodeHandle private_nh_;
    ros::NodeHandle nh_;
    
    
    void WorkerThread()
    {
      ros::Rate rate(10.0);
      tf2_ros::Buffer tfBuffer;
      tf2_ros::TransformListener tfListener(tfBuffer);
      geometry_msgs::TransformStamped transformStamped;
      while(ros::ok())
      {
        try{
          transformStamped = tfBuffer.lookupTransform(global_frame_id_, base_frame_id_, 
 													ros::Time(0),ros::Duration(0.1));
        }catch(tf2::TransformException &ex){
          
          ROS_WARN("%s",ex.what());
          continue;
        }
        
        
        tf::Transform map_robot_transform(tf::Quaternion (transformStamped.transform.rotation.x,
                                  transformStamped.transform.rotation.y,transformStamped.transform.rotation.z,
                                  transformStamped.transform.rotation.w),
                                  tf::Vector3 (transformStamped.transform.translation.x,transformStamped.transform.translation.y,
                                  transformStamped.transform.translation.z));
        tf::Quaternion q; q.setRPY(origin_[2],0,0);
        tf::Transform map_grid_transform(q, 
                                  tf::Vector3 (origin_[0],origin_[1],0));
        q.setRPY(3.14,0,0);
        tf::Transform grid_arr_transform(q,
                                  tf::Vector3 (0,resolution_*height_,0));
        
        static tf::TransformBroadcaster br;
        tf::Transform transform1;
        transform1.setOrigin( tf::Vector3(map_grid_transform.getOrigin().getX(), map_grid_transform.getOrigin().getY(), 0.0) );
        //tf::Quaternion q;
        q.setRPY(0, 0, origin_[2]);
        transform1.setRotation(q);
        br.sendTransform(tf::StampedTransform(transform1, ros::Time::now(), "map", "grid"));

        // tf::Transform transform2;
        // transform2.setOrigin( tf::Vector3(grid_arr_transform.getOrigin().getX(), grid_arr_transform.getOrigin().getY(), 0.0) );
        // transform2.setRotation(grid_arr_transform.getRotation());
        // br.sendTransform(tf::StampedTransform(transform2, ros::Time::now(), "grid", "arr"));


        //tf::Transform res = grid_arr_transform.inverse() * map_grid_transform.inverse() * map_robot_transform;
        tf::Transform res = map_grid_transform.inverse() * map_robot_transform;
        tf::Vector3 p = res.getOrigin();
        double map_robot_x = map_robot_transform.getOrigin().getX();
        double map_robot_y = map_robot_transform.getOrigin().getY();
        double grid_robot_x = (map_grid_transform.inverse() * map_robot_transform).getOrigin().getX();
        double grid_robot_y = (map_grid_transform.inverse() * map_robot_transform).getOrigin().getY();
        double x = p.getX();
        double y = p.getY();

        bool pub_ = false;
   
        for(int i=0;i<v_zone_map_resp_.size();i++)
        {
          // scaling and transform to 1-D  
          int x_idx = 1/resolution_ * x;
          int y_idx = 1/resolution_ * width_ * y;
          y_idx = y_idx - y_idx%width_; // this is so crucial
          int idx = x_idx + y_idx;
          if (idx >= 0 && idx < width_*height_ ) // occupied area
          {
            if(v_zone_map_resp_[i].map.data[idx])
            {
              std_msgs::String msg;
              msg.data = zone_topic_name_[i];
              robot_zone_pub_.publish(msg);
              pub_=true;
            }
          }
        }
        if(!pub_)
        {
          std_msgs::String msg;
          msg.data = "None";
          robot_zone_pub_.publish(msg);
        }
       
        rate.sleep();
      }
        
    };
    boost::thread zone_thr_;

    string map_metadata_;
    ros::Subscriber map_metadata_sub_;
    double origin_[3];
    double resolution_;
    uint32_t width_;
    uint32_t height_;

    string global_frame_id_;
    string base_frame_id_;
    string zone_folder_path_;
    bool each_pub_;
    
    std::vector<std::string> zone_topic_name_;
    std::vector<std::string> zone_img_path_;
    std::vector<nav_msgs::MapMetaData> v_meta_data_message_;
    std::vector<ros::Publisher> v_zone_meta_pub_;
    std::vector<nav_msgs::GetMap::Response> v_zone_map_resp_;

    std::vector<nav_msgs::OccupancyGrid> v_zone_map_data_message;
    std::vector<ros::Publisher> v_zone_map_pub_;
    
    
    nav_msgs::MapMetaData meta_data_message_;
    ros::Publisher zone_meta_pub_;
    nav_msgs::GetMap::Response zone_map_resp_;
    ros::Publisher zone_map_pub_;

    ros::Publisher robot_zone_pub_;

    void map_callback(const nav_msgs::MapMetaData::ConstPtr& msg){
        origin_[0] = msg->origin.position.x;
        origin_[1] = msg->origin.position.y;
        tf::Quaternion q(
          msg->origin.orientation.x,
          msg->origin.orientation.y,
          msg->origin.orientation.z,
          msg->origin.orientation.w);
        tf::Matrix3x3 m(q);
        double roll, pitch, yaw;
        m.getRPY(roll, pitch, yaw);
        origin_[2] = yaw;

        resolution_ = msg->resolution;
        width_ = msg->width; 
        height_ = msg->height;
        
        ROS_INFO("Heard Map MetaData ! ");
        
        loadZoneMap(zone_folder_path_,origin_,resolution_);
        
        combineMap(meta_data_message_,zone_map_resp_,v_zone_map_resp_); // make one costmap and publish for visualization
    }

    void combineMap(nav_msgs::MapMetaData& meta_data_message,nav_msgs::GetMap::Response& zone_map_resp,const std::vector<nav_msgs::GetMap::Response>& v_zone_map_resp_)
    {
      zone_map_resp = v_zone_map_resp_.front();
      //meta_data_message = v_zone_map_resp_.front().map.info;
      for(auto& map_resp : v_zone_map_resp_)
      {
        
        for(int i=0;i<width_*height_;i++)
        {
          zone_map_resp.map.data[i] = zone_map_resp.map.data[i] > 0 ? zone_map_resp.map.data[i] : map_resp.map.data[i];
        }
          // zone_map_resp.map.data[i] = zone_map_resp.map.data[i] > 0 ? 120 : 0; 
      }
        
      zone_map_pub_.publish(zone_map_resp.map);
      //zone_meta_pub_.publish(meta_data_message);

      /* for Visualization Purpose GridMap 1D Data Sequence*/
      // for(auto& map_resp : v_zone_map_resp_)
      //     for(int i=0;i<width_*height_;i++){
      //       zone_map_resp.map.data[i] = 100;
      //       usleep(50);
      //       zone_map_pub_.publish(zone_map_resp);
      //     }
      
      
      
      
    }


    void loadZoneMap(std::string z_path,double* origin,double resolution)
    {
      if(zone_folder_path_.empty())
      {
        ROS_WARN("The Zone Folder path param cannot be an empty string.");
        exit(-1);
      }
      boost::filesystem::path mapfpath(zone_folder_path_);
      if (!mapfpath.is_absolute()){
        ROS_WARN("Specify Zone Folder Path as Absolute Path");
        ROS_ERROR("Zone_server could not open %s.", zone_folder_path_.c_str());
        exit(-1);
      }
      zone_img_path_.clear();
      zone_topic_name_.clear();
      v_zone_map_resp_.clear();
      for(auto const& dir_entry : boost::filesystem::directory_iterator{mapfpath})
      {
        zone_img_path_.emplace_back(dir_entry.path().string());
      }
        
      for(const std::string& img_path : zone_img_path_) // extract topic name by each pgm files. ex) '~~~/zoneFolder/{name}.pgm' -> {name}
      {
        size_t s,e;
        s=e=0;
        for(int i=img_path.size()-1;i>=0;i--)
        {
          if(img_path[i] == '.')  e = i;
          else if(img_path[i] == '/')  s = i;
          if(s>0 && e>0)  break;
        }
        zone_topic_name_.emplace_back(img_path.substr(s+1,e-s-1));
      }
      for(int i=0;i<zone_img_path_.size();i++)
      {
        ROS_INFO("Zone Pgm : %s",zone_img_path_[i].c_str());
        ROS_INFO("Assigned Topic Name : %s",zone_topic_name_[i].c_str());
        if(each_pub_) //Publish latched topics
        {
          v_zone_map_pub_.emplace_back(nh_.advertise<nav_msgs::OccupancyGrid>(zone_topic_name_[i],1,true));
          v_zone_meta_pub_.emplace_back(nh_.advertise<nav_msgs::MapMetaData>(zone_topic_name_[i]+"_metadata",1,true));
        }
        if(!loadMapFromValues(zone_img_path_[i],resolution_,origin_,color[i%5]))
        {
          exit(-1);
        }
      }

    }
    bool loadMapFromValues(std::string map_file_name,double resolution,double origin[3],unsigned char colorVal)
    {
      int negate = 0;
      double occ_th = 0.65;
      double free_th = 0.196;
      MapMode mode = TRINARY;
      nav_msgs::MapMetaData meta_data_message_;
      nav_msgs::GetMap::Response map_resp_;
      try 
      {
        map_server::loadMapFromFile(&map_resp_, map_file_name.c_str(),
                                    resolution, negate, occ_th, free_th,
                                    origin, mode);
      } catch(std::runtime_error& e) 
      {
        ROS_WARN("Error While Loading map from image \"%s\"", map_file_name.c_str());
        ROS_ERROR("%s", e.what());
        return false;
      }
      ros::Time::waitForValid();
      map_resp_.map.info.map_load_time = ros::Time::now();
      map_resp_.map.header.frame_id = global_frame_id_;
      map_resp_.map.header.stamp = ros::Time::now();
      meta_data_message_ = map_resp_.map.info;
      ROS_INFO("READ PGM : %s",map_file_name.c_str());
      ROS_INFO("Read a %d X %d map @ %.3lf m/cell",
              map_resp_.map.info.width,
              map_resp_.map.info.height,
              map_resp_.map.info.resolution);
      

      // empty area -> 100
      // shape area -> 0
      // for visualization, specify value 
      for(int i=0;i<width_*height_;i++){
        if(map_resp_.map.data[i] > 0 ) // empty area parsing to zero data
          map_resp_.map.data[i] = 0;
        else 
          map_resp_.map.data[i] = colorVal;

      }


      if(each_pub_) // Publish each map latched topics
      {
        v_zone_map_pub_.back().publish(map_resp_.map);
        v_zone_meta_pub_.back().publish(meta_data_message_);
      }
      v_meta_data_message_.emplace_back(meta_data_message_);
      v_zone_map_resp_.emplace_back(map_resp_);
      v_zone_map_data_message.emplace_back(map_resp_.map);

      return true;
    }


};

int main(int argc, char **argv){

  ros::init(argc, argv, "robot_zone_sever", ros::init_options::AnonymousName);
  
  RobotZoneServer rzs;
  ros::spin();

  return 0;
}