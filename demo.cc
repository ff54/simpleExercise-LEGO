#include <iostream>
#include "utils.hh"
#include "deviceManager.hh"
using namespace std;
int main()
{
    shared_ptr<device::DeviceManager> d1 = device::DeviceManager::instance();
    /* TODO
    string cmd;
    while (true) {
        printf("%s>", d1->getName().c_str());
        getline(cin, cmd);
    }
    */
    d1->setName("test1");
    d1->start();
    sensor::SensorDataMap s = {{"a", "123"}, {"b", "4.34"}};
    auto ev = make_shared<sensor::SensorEvent>("sensor1", s);
    d1->pushEventToDevice(ev);
    std::this_thread::sleep_for(1000ms);
    d1->pushEventToDevice(ev);
    device::DeviceManager::FetchFunc func =  [](const string& a, sensor::SensorDataMap&) {
        utils::Log(utils::VERBOSE, string("fetching data from sensor1").c_str());
        return;
    };
    d1->registerPollingTask("sensor1",func, 1);
    std::this_thread::sleep_for(4000ms);
    d1->stop();
    return 0;
}