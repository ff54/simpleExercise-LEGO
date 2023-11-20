#include <iostream>
#include <map>
#include "utils.hh"
#include "deviceManager.hh"
#include "sensorSimulator.hh"
using namespace std;
class Sensor1 : public sensor::SensorSimulator {
public:
    Sensor1(string name, shared_ptr<device::DeviceManager> dm, int freq) : SensorSimulator(name, dm, freq){}
    void myJob() {
        sensor::SensorDataMap s = {{"sensor1/A", to_string(rand() % 1000)}, {"sensor1/B", to_string(rand() % 1000)},
                                    {"sensor1/C", to_string(rand() % 1000)}};
        auto ev = make_shared<sensor::SensorEvent>(m_name, s);
        m_dm->pushEventToDevice(ev);
    }
};

class Sensor2 : public sensor::SensorSimulator {
public:
    Sensor2(string name, shared_ptr<device::DeviceManager> dm, int freq) : SensorSimulator(name, dm, freq){}
    void myJob() {
        sensor::SensorDataMap s = {{"sensor2/Length", to_string(rand() % 100)}, {"sensor2/Width", to_string(rand() % 100)}};
        auto ev = make_shared<sensor::SensorEvent>(m_name, s);
        m_dm->pushEventToDevice(ev);
    }
};


void help() {
    cout<<"'start'  to start the device "<<endl;
    cout<<"'stop'   to stop the device"<<endl;
    cout<<"'post'   to post an event to device"<<endl;
    cout<<"'update' to update the device"<<endl;
    cout<<"'version'to show the version of device"<<endl;
    cout<<"'ss'     to start sensor simulators"<<endl;
    cout<<"'stop ss'to stop sensor simulators"<<endl;
    cout<<"'fetch'  to register a repeated task to fetch data by device"<<endl;
    cout<<"'exit'   to exit the app"<<endl;
}
shared_ptr<device::DeviceManager> d1;
shared_ptr<Sensor1> sensor1;
shared_ptr<Sensor2> sensor2;

void start() {
    d1->start();
}

void ss() {
    utils::Log(utils::INFO, "start 2 Sensor, they will keep sending evnets to device in a fixed frequency.");
    if (!sensor1)
        sensor1 = make_shared<Sensor1>("sensor1", d1, 3);
    sensor1->start();
    if (!sensor2)
        sensor2 = make_shared<Sensor2>("sensor2", d1, 4);
    sensor2->start();
}

void stop() {
    d1->stop();
}
void stopss() {
    sensor1.reset();
    sensor2.reset();
}

void fetch() {
    device::DeviceManager::FetchFunc func =  [](const string& a, sensor::SensorDataMap&) {
        utils::Log(utils::VERBOSE, "Fetching data from sensor1");
        return;
    };
    d1->registerPollingTask("sensor1",func, 1);
}

void update() {
    d1->updateDevice();
}


int main()
{
    d1 = device::DeviceManager::instance();
    d1->setName("test1");
    sensor1 = make_shared<Sensor1>("sensor1", d1, 1);
    sensor2 = make_shared<Sensor2>("sensor2", d1, 2);
    map<string, std::function<void()>> cmdList;
    cmdList["help"] = help;
    cmdList["start"] = start;
    cmdList["stop"] = stop;
    cmdList["ss"] = ss;
    cmdList["sstop"] = stopss;
    cmdList["fetch"] = fetch;
    cmdList["update"] = update;
    string cmd;
    utils::Log(utils::INFO, "type 'help' for more information");
    while (true) {
        printf("%s>", d1->getName().c_str());
        getline(cin, cmd);
        if (cmd == "exit") break;
        if (cmdList.count(cmd) > 0) {
            cmdList[cmd]();
        }
        else {
            utils::Log(utils::WARNING, "Unknown cmd, type 'help' for more information");
        }
    }
    sensor1.reset();
    sensor2.reset();
    d1.reset();
    return 0;
}