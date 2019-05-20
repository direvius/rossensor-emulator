## Velodyne VLP-16 emulator

Takes a rosbag file with VLP-16 messages and plays it back to UDP like if it was a physical device.

KNOWN ISSUES:
1. Possibility of not following the schedule accurately (needs investigation).
2. Parameters are hardcoded
3. Source port is random and in original PCAP it equals dst port, 2368
4. They say they use multicast in docs, but I couldn't yet find out what does it mean exactly. Seems more like broadcast
5. No error handling in code
6. No tests
7. Move debug output to debug build
8. Add logging (but not too much, packets per second, maybe)
9. NOT TESTED WITH REAL ROS (yet)

testdump.pcap is a capture obtained by playing back a velodyne.bag file.

## Build

Build in ROS kinetic environment. Install ROS deps from ROS tutorial (below).
Install velodyne deps by following the tutorial (link below).

```
./cmake
make
```

## Commands and links

```
$ docker start -i musing_swirles
root@d38cd576e467:/# roscore

# connect another terminal to docker:
$ docker exec -it musing_swirles bash
root@d38cd576e467:/# . /opt/ros/kinetic/setup.bash

# play udp dump:
rosrun velodyne_driver velodyne_node _pcap:=dump.pcap

# listen ros topic to console:
rostopic echo /velodyne_packets

# record velodyne packets to rosbag (inside bagfile directory):
rosbag record /velodyne_packets

# build C++ with ROS libraries (from docker container):
g++ src/main.cpp -I /opt/ros/kinetic/include/ -L/opt/ros/kinetic/lib/ -lrosbag -lrosbag_storage -lroscpp -lrostime -lcpp_common -lroscpp_serialization

# list objects in .so
nm --demangle --dynamic --defined-only --extern-only /opt/ros/kinetic/lib/*.so

# listen multicast
socat UDP4-RECVFROM:2368,ip-add-membership=239.255.0.1:0.0.0.0,fork - |hexdump
```

ROS docker tutorial: http://wiki.ros.org/docker/Tutorials/Docker
Velodyne tutorial: http://wiki.ros.org/velodyne/Tutorials/Getting%20Started%20with%20the%20Velodyne%20VLP16
Velodyne driver wiki: http://wiki.ros.org/velodyne_driver
Recording and playing data in ROS: http://wiki.ros.org/ROS/Tutorials/Recording%20and%20playing%20back%20data
Velodyne driver sources: https://github.com/ros-drivers/velodyne
Velodyne datasheets: https://velodynelidar.com/downloads.html
Velodyne pcap: https://data.kitware.com/#folder/5b7fff608d777f06857cb539

ROS Architecture: https://hub.packtpub.com/ros-architecture-and-concepts/

C++ linker concepts: https://www.toptal.com/c-plus-plus/c-plus-plus-understanding-compilation
