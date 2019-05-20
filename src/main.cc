#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <velodyne_msgs/VelodyneScan.h>

// #include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <thread>

// #define foreach BOOST_FOREACH

#include <iostream>

using namespace boost::asio;

void sendOverUDP(velodyne_msgs::VelodynePacket& p) {}

inline long long time_to_ns(int sec, int nsec) {
    return sec * 1000000000LL + nsec;
}

int main() {
    io_service service;
    ip::udp::endpoint ep( ip::address::from_string("127.0.0.1"), 2368);

    rosbag::Bag bag;
    bag.open("velodyne.bag");

    auto start = std::chrono::high_resolution_clock::now();
    long long first_stamp = 0;

    for(rosbag::MessageInstance const m: rosbag::View(bag))
    {
        // std::cout << m.getTime() << std::endl;
        // std::cout << m.getDataType() << std::endl;
        velodyne_msgs::VelodyneScan::ConstPtr i = m.instantiate<velodyne_msgs::VelodyneScan>();
        if (i != NULL)
            // std::cout << i->header << std::endl;
            for(velodyne_msgs::VelodynePacket p: i->packets) {
                long long nanos = time_to_ns(p.stamp.sec, p.stamp.nsec);
                if(first_stamp == 0) {
                    first_stamp = nanos;
                }
                std::this_thread::sleep_until(start + std::chrono::nanoseconds(nanos - first_stamp));
                ip::udp::socket sock(service);
                sock.connect(ep);
                sock.send(buffer(p.data, sizeof(p.data)));
                sock.close();
            }
    }

    bag.close();
}
