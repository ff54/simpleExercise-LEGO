#ifndef DEVICE_MANAGER_HH
#define DEVICE_MANAGER_HH
/**
 * simple device manager  which allow its underlying sensor to post events (sensor data)
 *
 * This class holds a simple evt queue to listen events from sensor,
 * a scheduler to fetch data from sensor and local cache to keep all historical device states
 */
#include <string> 
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>

#include "sensorSimulator.hh"
namespace device {

class RepeatedTask;
struct NameTimePair;

enum Status {
    UNKNOW = -1,
    ON,
    OFF,
    PAUSE,
};

struct Event {
    Event(sensor::SensorDataMap& input, const std::string& name);
    Event(const std::string& name);
    //move constructor
    Event(Event&& other);
    Event(Event& rhs);
    ~Event(){};
    std::chrono::system_clock::time_point timeStamp;
    sensor::SensorDataMap data;
    std::string sensorName;
};

class DeviceManager
{
public:
    static std::shared_ptr<DeviceManager> instance();
    ~DeviceManager();
    DeviceManager(DeviceManager& rhs) = delete;
    DeviceManager(DeviceManager&& rhs) = delete;

    void PushDeviceEvent(Event evt);
    void registerPollingTask();
    void getStatus() const;
    void start();
    void stop();
    void setName(const std::string& name) {m_name = name;};
private:
    DeviceManager();//singletone
    void run();
    void runScheduler();
    std::condition_variable m_condition; 
    std::mutex m_mutex;
    Status m_status = OFF;
    std::thread m_thread;
    std::thread m_scheduler;
    std::queue<Event> m_queue;
    std::priority_queue<RepeatedTask> m_taskQ;
    bool m_running = false;
    std::unordered_map<std::string, std::pair<std::time_t, sensor::SensorDataMap>> m_cache; //keep sensor name, sensor data and last updated time
    std::string m_name{"TestDevice"};
};

} //end of namespace device
#endif