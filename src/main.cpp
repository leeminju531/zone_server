
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
// enum Color {
//   COLOR_A = 10,
//   COLOR_B = 50,
//   COLOR_C = 90,
//   COLOR_D = 110,
//   COLOR_E = 140,
//   COLOR_F = 180,
// };
// int color[6] = {COLOR_A,COLOR_B,COLOR_C,COLOR_D,COLOR_E,COLOR_F}
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

        tf::Transform transform2;
        transform2.setOrigin( tf::Vector3(grid_arr_transform.getOrigin().getX(), grid_arr_transform.getOrigin().getY(), 0.0) );
        transform2.setRotation(grid_arr_transform.getRotation());
        br.sendTransform(tf::StampedTransform(transform2, ros::Time::now(), "grid", "arr"));


        tf::Transform res = grid_arr_transform.inverse() * map_grid_transform.inverse() * map_robot_transform;
        res = map_grid_transform.inverse() * map_robot_transform;
        tf::Vector3 p = res.getOrigin();
        double map_robot_x = map_robot_transform.getOrigin().getX();
        double map_robot_y = map_robot_transform.getOrigin().getY();
        double grid_robot_x = (map_grid_transform.inverse() * map_robot_transform).getOrigin().getX();
        double grid_robot_y = (map_grid_transform.inverse() * map_robot_transform).getOrigin().getY();
        double x = p.getX();
        double y = p.getY();

        std::cout <<"map_robot : "<<" ( " <<map_robot_x<<","<<map_robot_y<<")"<< std::endl;
        std::cout <<"grid_robot : "<<" ( " <<grid_robot_x<<","<<grid_robot_y<<")"<< std::endl;
        std::cout <<"arr_robot : "<<" ( " <<x<<","<<y<<")"<<  std::endl;

        // std::cout <<"at grid map : "<<" ( " <<1/resolution_ * x<<","<<1/resolution_ * width_ * y<<")"<<  std::endl;
        // std::cout <<" grid sum : "<<1/resolution_ * x + 1/resolution_ * width_ * y << std::endl;

        // int a1 = 1/resolution_ * width_ * y;
        // int x1 = 1/resolution_ * x;
        // std::cout <<"at grid map2 : "<<" ( " <<x1<<","<<a1<<")"<<  std::endl;
        // std::cout <<" grid sum : "<<x1+a1<< std::endl;
        

        //std::cout <<" v_zone_map_resp_ .size : " << v_zone_map_resp_.size()<< std::endl;
        bool pub_ = false;
        int a = 1/resolution_ * (x + width_ * y);
        cout << "pure idx : "<< 1/resolution_ * (x + width_ * y)<<std::endl;
        cout << "idx : "<< a<<std::endl;
        cout << "width * height : "<<width_*height_ << std::endl;
        

        // int idx = 1/resolution_ * (9 + width_ * 9);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);
        // // usleep(1000);
        //  idx = 1/resolution_ * (9 + width_ * 10);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);
        // idx = 1/resolution_ * (9.5 + width_ * 9.5);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);
        // // usleep(1000);
        //  idx = 1/resolution_ * (10 + width_ * 9);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);
        // // usleep(1000);
        //  idx = 1/resolution_ * (10 + width_ * 10);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);

        // idx = 1/resolution_ * (9.25123 + width_ * 9.24967);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);

        // idx = 1/resolution_ * (9.38 + width_ * 9.911);
        // v_zone_map_resp_[0].map.data[idx] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);
        // x = 9.38;
        // y = 9.911;
        // std::cout <<"(9.38,9.911) at grid map : "<<" ( " <<1/resolution_ * x<<","<<1/resolution_ * width_ * y<<")"<<  std::endl;
        // std::cout <<" grid sum : "<<1/resolution_ * x + 1/resolution_ * width_ * y << std::endl;

        // int a1 = 1/resolution_ * width_ * y;
        // a1 = a1 - a1%width_;
        // int x1 = 1/resolution_ * x;
        
        // v_zone_map_resp_[0].map.data[a1+x1] = 120;
        // zone_map_pub_.publish(v_zone_map_resp_[0].map);
        // std::cout <<"at grid map2 : "<<" ( " <<x1<<","<<a1<<")"<<  std::endl;
        // std::cout <<" grid sum : "<<x1+a1<< std::endl;

        for(int i=0;i<v_zone_map_resp_.size();i++)
        {
          int idx = 1/resolution_ * (x + width_ * y); // scaling and transform to 1-D  
          // if (idx >= 0 && idx < width_*height_ && v_zone_map_resp_[i].map.data[idx] > 0 ) // occupied area
          // {
          //   std_msgs::String msg;
          //   msg.data = zone_topic_name_[i];
          //   robot_zone_pub_.publish(msg);
          //   pub_=true;
          //   // std::cout <<" pubed " << std::endl;
          //   break;
          // }
          int a1 = 1/resolution_ * width_ * y;
          int x1 = 1/resolution_ * x;
          if (idx >= 0 && idx < width_*height_ ) // occupied area
          {
            //v_zone_map_resp_[i].map.data[idx];
            std::cout << "resolution  : "<<resolution_ << std::endl;
            std::cout << "zone_topic_name_["<<i<<"] : "<<zone_topic_name_[i] << std::endl;
            std::cout << "v_zone_map_resp_["<<i<<"].map.data[idx] : "<<(int)v_zone_map_resp_[i].map.data[idx] <<std::endl;
            std::cout << "v_zone_map_data_message["<<i<<"].data[idx] : "<<(int)v_zone_map_data_message[i].data[idx] <<std::endl;
            if(v_zone_map_resp_[i].map.data[a1 - a1%width_+x1])
            {
              std::cout << "ddddddd in area : " << zone_topic_name_[i] << std::endl;
              
              std_msgs::String msg;
              msg.data = zone_topic_name_[i];
              robot_zone_pub_.publish(msg);
              pub_=true;
            }
            //v_zone_map_resp_[i].map.data[idx] = 120;
            
            
            

            // std::cout <<" pubed " << std::endl;
            
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
      //meta_data_message = v_zone_map_resp_.front().map.info();
      // for(auto& map_resp : v_zone_map_resp_)
      //   for(int i=0;i<width_*height_;i++){
      //     zone_map_resp.map.data[i] = zone_map_resp.map.data[i] > 0 ? zone_map_resp.map.data[i] : map_resp.map.data[i]; 
          
      //   }
      // for(auto& map_resp : v_zone_map_resp_)
      //     for(int i=0;i<width_*height_;i++){
      //       zone_map_resp.map.data[i] = 100;
      //       usleep(50);
      //       zone_map_pub_.publish(zone_map_resp);
      //     }
      // for(int i=0;i<10;i++)
      // {
      //   for(int j=0;j<5;j++)
      //   {
      //     if(width_*height_ > 1/resolution_ * (i + width_ * j)){
      //       zone_map_resp.map.data[1/resolution_ * (i + width_ * j)] = 150;
      //       usleep(50);
      //       zone_map_pub_.publish(zone_map_resp);
      //     }
            
      //   }
      // }
      //zone_meta_pub_.publish(meta_data_message);
      
      
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
        if(!loadMapFromValues(zone_img_path_[i],resolution_,origin_))
        {
          exit(-1);
        }
      }

    }
    bool loadMapFromValues(std::string map_file_name,double resolution,double origin[3])
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
      

      // for visualization, specify value 
      for(int i=0;i<width_*height_;i++){
        if(map_resp_.map.data[i] == -1) // unknown
          map_resp_.map.data[i] = 0;
        // else if(map_resp_.map.data[i] == 0) // free 
        //   map_resp_.map.data[i] = 99;
        else // occupied
          map_resp_.map.data[i] = 99;

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