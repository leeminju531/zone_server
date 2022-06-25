# zone_server
[학부졸업논문_이민주.pdf](https://github.com/leeminju531/zone_server/files/8984623/_.pdf)



## youtube
[![zone server](https://img.youtube.com/vi/w8JvzQT0lYw/0.jpg)](https://www.youtube.com/watch?v=w8JvzQT0lYw)

## Dependency
    map_server

(QT Related)    

    sudo apt-get udpate
    sudo apt-get install build-essential
    sudo apt-get install qt5-default
    sudo apt-get install qt5-doc qt5-doc-html qtbase5-doc-html qtbase5-example
    

## QT Build 
cd $(find zone_server)/QT/undo    

    qmake
    make

- to load user grid map as GUI background, modify 'icons/map.pgm' to user pgm map path   
 below files line 
 
      document.cpp , undo.qrc //inside $(find zone_server)/QT/undo
 

## zone server 

### subscribed Topics
  ***~map_metadata (nav_msgs/MapMetaData)***     
  >subscibe metadata by map_server to calculate relation between array and map 
---      
### published Topic

  ***~zone_cost_map (nav_msgs/OccupancyGrid)***     
  >publish user generative zone costmap as latched topic  
    
  ***~zone_metadata (nav_msgs/MapMetaData)***       
  >latched topic
    
  ***~robot_zone (std_msgs/String)***       
  >publish robot zone. each publishing name equal to pgm file name
---
### Parameters

  ***~zone_path(string, default : "")*** 
  >QT result pgms folder path    
   
  ***~map_metadata (string , default : "map_metadata")***     
  ***~global_frame_id (string, default : "map")***    
  ***~base_frame_id (string , default : "base_footprint)***   
  
  
