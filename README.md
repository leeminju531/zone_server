# zone_server
ROS Occupancy Map Zone Notification for flexible navigation param tunning 

### Package Summary
One of the most flexible aspect about ROS navigation is dynamic reconfiguration, since different parameter setup might be more helpful for certain situations. nevertheless, 이미 알고있는 맵 정보에 대한 파라미터 수정이 decision making단계 같은 하이레벨 테스크에 수행되는 것은 로봇의 움직임이 부자연스럽게 보이게 하고 컨셉적으로 부적합해보인다. 사용자 generate 영역을 알림으로써 미들 task에서 튜닝을 도울 수 있다.

### youtube
[![zone server](https://img.youtube.com/vi/w8JvzQT0lYw/0.jpg)](https://www.youtube.com/watch?v=w8JvzQT0lYw)

### Dependency
map_server

(QT Related)
sudo apt-get udpate
sudo apt-get install build-essential
sudo apt-get install qt5-default
sudo apt-get install qt5-doc qt5-doc-html qtbase5-doc-html qtbase5-example

### QT Build 
cd $(find zone_server)/QT/undo
qmake
make


UI의 백그라운드로 사용자 맵을 로드하기 위해서 
빌드 전에 cd $(find zone_server)/QT/undo 하위 
document.cpp
undo.qrc 파일 안
map.pgm 파일을 사용자 map으로 변경하기 

### zone server 

subscribed Topics
  ~map_metadata
      grid 맵 정보와 map 사이 계산을 위해 사용함. 사용자의 map_server로 부터 publish 된 정보를 subscribe
      
published Topic
  ~zone_cost_map (nav_msgs/OccupancyGrid)
    사용자 수정한 zone을 색으로 구분해 publish 한다.
  ~zone_metadata (nav_msgs/MapMetaData)   - latched topic
    
  ~robot_zone (std_msgs/String) 
    로봇의 위치를 퍼블리쉬한다. 영역의 이름은 읽어들인 pgm파일의 이름과 같음.
    
Parameters
  ~zone_path(string, default : "") : QT결과 pgm 파일들이 저장된 폴더 경로
  ~map_metadata (string , default : "map_metadata")
  ~global_frame_id (string, default : "map")
  ~base_frame_id (string , default : "base_footprint)
  
  
