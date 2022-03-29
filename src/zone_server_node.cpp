/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Author: Brian Gerkey */

#define USAGE "\nUSAGE: map_server <map.yaml>\n" \
              "  map.yaml: map description file\n" \
              "DEPRECATED USAGE: map_server <map> <resolution>\n" \
              "  map: image file to load\n"\
              "  resolution: map resolution [meters/pixel]"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <boost/filesystem.hpp>

#include "ros/ros.h"
#include "ros/console.h"
#include "map_server/image_loader.h"
#include "nav_msgs/MapMetaData.h"
#include "nav_msgs/LoadMap.h"
#include "yaml-cpp/yaml.h"
#include <iostream>
#ifdef HAVE_YAMLCPP_GT_0_5_0
// The >> operator disappeared in yaml-cpp 0.5, so this function is
// added to provide support for code written under the yaml-cpp 0.3 API.
template<typename T>
void operator >> (const YAML::Node& node, T& i)
{
  i = node.as<T>();
}
#endif

class MapInfo{
public: 
  MapInfo(const std::string& path_map_yaml){
    if (!loadMapFromYaml(path_map_yaml)){
      ROS_WARN("Zone Map Server Only Support Loading From Yaml Format");
      exit(-1);
    }
  }
  

  MapMode mode = TRINARY;
  double res;
  int negate;
  double occ_th;
  double free_th;
  double origin[3];
  std::vector<std::string> zone_topic_name_;
  std::vector<std::string> zone_img_path_;
  int img_cnt_;
  
  bool loadMapFromYaml(std::string path_to_yaml){
    std::ifstream fin(path_to_yaml.c_str());
    if (fin.fail()) {
      ROS_ERROR("Map_server could not open %s.", path_to_yaml.c_str());
      return false;
    }
    #ifdef HAVE_YAMLCPP_GT_0_5_0
        // The document loading process changed in yaml-cpp 0.5.
        YAML::Node doc = YAML::Load(fin);
    #else
        YAML::Parser parser(fin);
        YAML::Node doc;
        parser.GetNextDocument(doc);
    #endif

    try {
      doc["resolution"] >> res;
    } catch (YAML::InvalidScalar &) {
      ROS_ERROR("The map does not contain a resolution tag or it is invalid.");
      return false;
    }
    try {
      doc["negate"] >> negate;
    } catch (YAML::InvalidScalar &) {
      ROS_ERROR("The map does not contain a negate tag or it is invalid.");
      return false;
    }
    try {
      doc["occupied_thresh"] >> occ_th;
    } catch (YAML::InvalidScalar &) {
      ROS_ERROR("The map does not contain an occupied_thresh tag or it is invalid.");
      return false;
    }
    try {
      doc["free_thresh"] >> free_th;
    } catch (YAML::InvalidScalar &) {
      ROS_ERROR("The map does not contain a free_thresh tag or it is invalid.");
      return false;
    }
    try {
      std::string modeS = "";
      doc["mode"] >> modeS;

      if(modeS=="trinary")
        mode = TRINARY;
      else if(modeS=="scale")
        mode = SCALE;
      else if(modeS=="raw")
        mode = RAW;
      else{
        ROS_ERROR("Invalid mode tag \"%s\".", modeS.c_str());
        return false;
      }
    } catch (YAML::Exception &) {
      ROS_DEBUG("The map does not contain a mode tag or it is invalid... assuming Trinary");
      mode = TRINARY;
    }
    try {
      doc["origin"][0] >> origin[0];
      doc["origin"][1] >> origin[1];
      doc["origin"][2] >> origin[2];
    } catch (YAML::InvalidScalar &) {
      ROS_ERROR("The map does not contain an origin tag or it is invalid.");
      return false;
    }
    try{
      std::string mapfname;
      doc["zone_img_path"] >> mapfname;
      if(mapfname.size() == 0){
        ROS_ERROR("The zone_img_path tag cannot be an empty string.");
        return false;
      }
      boost::filesystem::path mapfpath(mapfname);
      if (!mapfpath.is_absolute()){
        boost::filesystem::path dir(path_to_yaml);
        dir = dir.parent_path();
        mapfpath = dir / mapfpath;
        mapfname = mapfpath.string();
      }
      for(auto const& dir_entry : boost::filesystem::directory_iterator{mapfpath}){
        //std::cout << dir_entry << std::endl;
        zone_img_path_.emplace_back(dir_entry.path().string());
      }
      img_cnt_ = zone_img_path_.size();
      // extract topic name 
      for(const std::string& img_path : zone_img_path_){
        size_t s,e;
        s=e=0;
        for(int i=img_path.size()-1;i>=0;i--){
          if(img_path[i] == '.')  e = i;
          else if(img_path[i] == '/')  s = i;
          if(s>0 && e>0)  break;
        }
        zone_topic_name_.emplace_back(img_path.substr(s+1,e-s-1));
      }
    }catch (YAML::InvalidScalar &) {
      ROS_ERROR("The map does not contain an image tag or it is invalid.");
      return false;
    }
    return true;
  }
};

class ZoneMapServer{

public:
  ZoneMapServer(MapInfo& mInfo) : zmInfo_(mInfo)
  {
    ros::NodeHandle private_nh("~");
    private_nh.param("frame_id", frame_id_, std::string("map"));
    for(int i=0;i<zmInfo_.img_cnt_;i++){
      v_map_pub_.emplace_back(nh_.advertise<nav_msgs::OccupancyGrid>(zmInfo_.zone_topic_name_[i], 1, true));
      v_metadata_pub_.emplace_back(nh_.advertise<nav_msgs::MapMetaData>( zmInfo_.zone_topic_name_[i] + "_metadata", 1, true));
      //When called this service returns a copy of the current map
      //get_map_service_ .emplace_back(nh_.advertiseService("static_" + zmInfo_.zone_topic_name_[i], &MapServer::mapCallback, this));
      if(!loadMapFromValues(zmInfo_.zone_img_path_[i],zmInfo_.res,zmInfo_.negate,zmInfo_.occ_th,zmInfo_.free_th,zmInfo_.origin,zmInfo_.mode,i))
      {
        exit(-1);
      }
    }
  }
private:
  MapInfo zmInfo_;
  ros::NodeHandle nh_;
  std::vector<ros::Publisher> v_map_pub_;
  std::vector<ros::Publisher> v_metadata_pub_;
  std::vector<ros::ServiceServer> get_map_service_;
  std::vector<ros::ServiceServer> change_map_srv_;
  std::string frame_id_;
  
  /** The map data is cached here, to be sent out to service callers
     */
  nav_msgs::MapMetaData meta_data_message_;
  nav_msgs::GetMap::Response map_resp_;

  /** Callback invoked when someone requests our service */
  // bool mapCallback(nav_msgs::GetMap::Request  &req,
  //                   nav_msgs::GetMap::Response &res )
  // {
  //   // request is empty; we ignore it

  //   // = operator is overloaded to make deep copy (tricky!)
  //   res = map_resp_;
  //   ROS_INFO("Sending map");

  //   return true;
  // }

  bool loadMapFromValues(std::string map_file_name, double resolution,
                           int negate, double occ_th, double free_th,
                           double origin[3], MapMode mode,int idx)
  {
    try {
      map_server::loadMapFromFile(&map_resp_, map_file_name.c_str(),
                                  resolution, negate, occ_th, free_th,
                                  origin, mode);
    } catch (std::runtime_error& e) {
      ROS_WARN("Error While Loading map from image \"%s\"", map_file_name.c_str());
      ROS_ERROR("%s", e.what());
      return false;
    }
    // To make sure get a consistent time in simulation
    ros::Time::waitForValid();
    map_resp_.map.info.map_load_time = ros::Time::now();
    map_resp_.map.header.frame_id = frame_id_;
    map_resp_.map.header.stamp = ros::Time::now();
    ROS_INFO("Read a %d X %d map @ %.3lf m/cell",
              map_resp_.map.info.width,
              map_resp_.map.info.height,
              map_resp_.map.info.resolution);
    meta_data_message_ = map_resp_.map.info;

    //Publish latched topics
    v_metadata_pub_[idx].publish( meta_data_message_ );
    v_map_pub_[idx].publish( map_resp_.map );

    
    return true;
  }
};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "zone_map_sever", ros::init_options::AnonymousName);
  ros::NodeHandle nh("~");
  
  std::string fname(argv[1]); // argv[1] : yaml path -> fname store yaml path

  try
  {
    MapInfo mInfo(fname); // read yaml file info
    ZoneMapServer zms(mInfo);
    ros::spin();
  }
  catch(std::runtime_error& e)
  {
    ROS_ERROR("zone_map_server exception: %s", e.what());
    return -1;
  }

  return 0;
}
