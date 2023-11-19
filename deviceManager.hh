#ifndef DEVICE_MANAGER_HH
#define DEVICE_MANAGER_HH
/*
 * =====================================================================================
 *
 *       Filename:  deviceManager.h
 *
 *    Description: simple device manager  which allow its underlying sensor to post
 *                 events (sensor data). This class holds a simple evt queue to
 *                 listen events from sensor, a scheduler to fetch data from sensor
 *                 and local cache to keep all historical device states
 *
 *        Version:  1.0
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Yaochuan Chen
 *
 * =====================================================================================
 */
#include <string> 
#include <condition_variable>
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <memory>

#include "sensorSimulator.hh"
#include "utils.hh"
namespace device {

struct RepeatedTask;

enum Status {
    UNKNOW = -1,
    ON,
    OFF,
    PAUSE,
};

struct DeviceEvent: public utils::Event  {
    DeviceEvent();
    DeviceEvent(const std::string& name, Status st);
    DeviceEvent(DeviceEvent& rhs);
    Status status;
};

class DeviceManager
{
public:
    typedef std::function<void(const std::string&, sensor::SensorDataMap&)> FetchFunc;

    static std::shared_ptr<DeviceManager> instance();
    ~DeviceManager();
    DeviceManager(DeviceManager& rhs) = delete;
    DeviceManager(DeviceManager&& rhs) = delete;

    void pushEventToDevice(std::shared_ptr<utils::Event> evt);
    void registerPollingTask(const std::string& sensorName, FetchFunc& func, int second);
    void start();
    void stop();
    void setName(const std::string& name) {m_name = name;};


    std::string getName() const {return m_name;}
    Status getStatus() const {return m_status;}
private:
    DeviceManager();//singletone
    void run();
    void runScheduler();
    std::condition_variable m_condition; 
    std::mutex m_mutex;
    std::mutex m_schedulerMutex;
    Status m_status = OFF;
    std::thread m_thread;
    std::thread m_scheduler;
    std::queue<std::shared_ptr<utils::Event>> m_queue;
    std::priority_queue<RepeatedTask> m_taskQ;
    std::vector<RepeatedTask> m_taskVector;
    bool m_running = false;
    //keep sensor name, sensor data and last updated time
    std::unordered_map<std::string, std::pair<std::time_t, sensor::SensorDataMap>> m_cache; 
    std::string m_name{"TestDevice"};
};

} //end of namespace device
#endif