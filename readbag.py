import rosbag
bag = rosbag.Bag('velodyne.bag')
for topic, msg, t in bag.read_messages(topics=['/velodyne_packets']):
    print msg
bag.close()