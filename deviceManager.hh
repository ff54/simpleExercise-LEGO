#ifndef DEVICE_MANAGER_HH
#define DEVICE_MANAGER_HH
/*
 * =====================================================================================
 *
 *       Filename:  deviceManager.hh
 *
 *    Description: simple device manager  which allow its underlying sensor to post
 *                 events (sensor data). This class holds a simple evt queue to
 *                 listen events from sensor, a scheduler to fetch data from sensor
 *                 and local cache to keep all historical device states
 *
 *        Version:  0.9.1
 *       Revision:  none
 *       Compiler:  g++
 *
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
    /**
     * \brief fetch function, users need to implement their own version and register at \ref registerPollingTask.
     * \note sensor::SensorDataMap& is output, DeviceManager will run the function repeatedly in anoter thread
     *       and save data at sensor::SensorDataMap to its local cache
     */
    typedef std::function<void(const std::string&, sensor::SensorDataMap&)> FetchFunc;

    /**
     * \brief get the DeviceManager instance.
     */
    static std::shared_ptr<DeviceManager> instance();
    ~DeviceManager();
    DeviceManager(DeviceManager& rhs) = delete;
    DeviceManager(DeviceManager&& rhs) = delete;

    /**
     * \brief sensor or deivece itself may push events to the DeviceManager eventLoop. DeviceManager will
     *        handle evt based on its type.
     */
    void pushEventToDevice(std::shared_ptr<utils::Event> evt);

    /**
     * \brief      register a function/task to fetch sensor data in a fixed frequency.
     * \param [in] sensorName the name of sensor
     * \param [in] func       function to fech data from sensor or some whereelse \ref FetchFunc
     * \param [in] n          run every n second
     */
    void registerPollingTask(const std::string& sensorName, FetchFunc& func, int n);
    void start();
    void stop();
    void setName(const std::string& name) {m_name = name;};
    void updateDevice();//TODO


    std::string getName() const {return m_name;}
    Status getStatus() const {return m_status;}
    std::string getVersion() const {return m_version;}
    std::string getLastUpdatedSensor() const; 
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
    std::string m_version;
};

} //end of namespace device
#endif