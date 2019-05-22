#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <velodyne_msgs/VelodyneScan.h>

// #include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <thread>

// #define foreach BOOST_FOREACH

#include <iostream>

using namespace boost::asio;
namespace po = boost::program_options;

void sendOverUDP(velodyne_msgs::VelodynePacket& p) {}

inline long long time_to_ns(int sec, int nsec) {
    return sec * 1000000000LL + nsec;
}

int main(int ac, char* av[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("endpoint,e", po::value< std::string >()->default_value("127.0.0.1"), "endpoint address")
        ("bagfile,b", po::value< std::string >()->default_value("velodyne.bag"), "bag file name")
        ("port,p", po::value<int>()->default_value(2368), "UDP target port")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    io_service service;
    ip::udp::endpoint ep;

    try {
        ep = ip::udp::endpoint(ip::address::from_string(vm["endpoint"].as< std::string >()), 2368);
    } catch(const std::exception& e) {
        std::cerr << "Failed to connect to endpoint. "  << e.what() << '\n';
        return 1;
    }
    

    rosbag::Bag bag;
    try {
        bag.open(vm["bagfile"].as< std::string >());
    } catch(const std::exception& e) {
        std::cerr << "Failed to open bag file. "  << e.what() << '\n';
        return 1;
    }

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
                try {
                    sock.connect(ep);
                    sock.send(buffer(p.data, sizeof(p.data)));
                    sock.close();
                } catch(const std::exception& e) {
                    std::cerr << "Failed to send UDP packet. " << e.what() << '\n';
                    sock.close();
                    return 1;
                } 
                
            }
    }

    bag.close();
    return 0;
}
