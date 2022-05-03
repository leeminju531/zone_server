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


빌드 전에 


### zone server 

subscribed Topics

published Topic

Parameters
