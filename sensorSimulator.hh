/**
 * simple sensor simulator interface
 *
 * This class starts a thread and invoke myJob function repeatedly in a fixed freq.
 * myJob function has to be implemented by user.
 */
#ifndef SENSOR_SIMULATOR_HH
#define SENSOR_SIMULATOR_HH
#include <string>
#include <unordered_map>
#include "utils.hh"
namespace device {
class DeviceManager;
}
namespace sensor {


//improvment: we use string to represent value now, but it can be replaced with std::any or template. Then we can pass
//any type of value.
typedef std::unordered_map<std::string, std::string> SensorDataMap;//<name of parameter, value>

struct SensorEvent: public utils::Event  {
    SensorEvent(const std::string& name, sensor::SensorDataMap& input);
    SensorEvent(const std::string& name);
    //move constructor
    SensorEvent(SensorEvent&& other);
    SensorEvent(SensorEvent& rhs);
    sensor::SensorDataMap data;
};

class SensorSimulator {
public:
    SensorSimulator(const std::string& name, std::shared_ptr<device::DeviceManager>& dm, int freq = 2);
    void start();
    virtual ~SensorSimulator();
    virtual void myJob() = 0;
protected:
    std::string m_name;
    std::shared_ptr<device::DeviceManager> m_dm;
    int m_freq;
private:
    void run();
    std::mutex m_schedulerMutex;
    std::thread m_scheduler;
    bool m_running = false;
};

}//end of namespace sensor
#endif