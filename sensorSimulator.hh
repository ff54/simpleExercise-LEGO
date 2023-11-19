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
namespace sensor {

//improvment: we use string to represent value now, but it can be replaced with std::any or template. Then we can pass
//any type of value.
typedef std::unordered_map<std::string, std::string> SensorDataMap;//<name of parameter, value>

class SensorSimulator {
public:
    SensorSimulator(const std::string& deviceName, int freq = 2);
    void start();
    virtual ~SensorSimulator();
    virtual void myJob() = 0;
private:
    void run();
    std::string m_name;
    int m_freq;
};

}//end of namespace sensor
#endif