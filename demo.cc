#include "deviceManager.hh"
using namespace std;
int main()
{
    shared_ptr<device::DeviceManager> d1 = device::DeviceManager::instance();
    d1->setName("test1");
    d1->start();
    sensor::SensorDataMap s = {{"a", "123"}, {"b", "4.34"}};
    device::Event ev(s, "sensor1");
    d1->PushDeviceEvent(ev);
    std::this_thread::sleep_for(1000ms);
    d1->PushDeviceEvent(ev);
    std::this_thread::sleep_for(1000ms);
    d1->stop();
    return 0;
}