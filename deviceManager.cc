#include <iostream>
#include <future>

#include "deviceManager.hh"
#include "utils.hh"
namespace device {
using namespace std;

struct RepeatedTask
{
    RepeatedTask(const string& sensorName, DeviceManager::FetchFunc& fetchFunc, int second):
        m_name(sensorName),
        m_func(move(fetchFunc)),
        m_second(second)
    {
    }

    RepeatedTask(const string& sensorName, DeviceManager::FetchFunc& fetchFunc, int second, const chrono::system_clock::time_point& time):
        m_name(sensorName),
        m_func(move(fetchFunc)),
        m_second(second),
        m_time(time)
    {

    }
    bool operator<(const RepeatedTask& rhs) const {
        return m_time > rhs.m_time;
    }

    void fetch(const string& sensorName, sensor::SensorDataMap& data) { m_func(sensorName, data); }
    string m_name;
    DeviceManager::FetchFunc m_func;
    int m_second;
    chrono::system_clock::time_point m_time;
};

DeviceEvent::DeviceEvent(const string& name, Status st): Event(name, utils::Event::DEVICEINFO), status(st) { }

DeviceEvent::DeviceEvent(DeviceEvent& rhs):Event(rhs), status(rhs.status){
}

shared_ptr<DeviceManager> DeviceManager::instance() {
    static shared_ptr<DeviceManager> manager(new DeviceManager());
    return manager;
}

DeviceManager::DeviceManager() {
}

DeviceManager::~DeviceManager() {
    if (m_running) stop();
}


void DeviceManager::start() {
    utils::Log(utils::DEBUG, "starting the deviceManager...");
    if (m_running) {
        return;
    }
    try{
        m_running = true;
        m_thread = thread([&](){run();});
        m_scheduler = thread([&](){runScheduler();});
    }
    catch (exception& e) {
        cout<<"caught an exception: "<<e.what()<<endl;
    }
    m_thread.detach();
    m_scheduler.detach();
    m_status = ON;
}

void DeviceManager::stop() {
    utils::Log(utils::DEBUG, "stopping the deviceManager...");
    unique_lock<mutex> lock(m_mutex);
    m_running = false;
    lock.unlock();
    m_condition.notify_one();    
    queue<shared_ptr<utils::Event>>().swap(m_queue);
    priority_queue<RepeatedTask>().swap(m_taskQ);
    m_status = OFF;
    utils::Log(utils::DEBUG, "DeviceManager stopped...");
}

void DeviceManager::run() {
    utils::Log(utils::INFO, "EventListener is running...");
    while (m_running)
    {
        unique_lock<mutex> lock(m_mutex);
        m_condition.wait(lock, [this](){ return (!m_queue.empty() || !m_running);});
        if(!m_running) {
            return;
        }
        shared_ptr<utils::Event> event = m_queue.front();
        if (event->evtType == utils::Event::SENSORINFO) {
            std::shared_ptr<sensor::SensorEvent> sensorEvt = std::dynamic_pointer_cast<sensor::SensorEvent> (event);
            if (!sensorEvt->data.empty()) {
                time_t t = chrono::system_clock::to_time_t(event->timeStamp);
                auto res = m_cache.emplace(sensorEvt->name, make_pair(t, sensorEvt->data));
                if (!res.second) {
                    res.first->second.first = t;
                    for (auto& p : sensorEvt->data) {
                        res.first->second.second[p.first] = p.second;
                    }
                }
                utils::Log(utils::INFO, "Consume an event...");
            }
        }
        else if (event->evtType == utils::Event::DEVICEINFO) {
            //TODO
        }
        m_queue.pop();
        lock.unlock();

    }
}

void DeviceManager::runScheduler() {
    if (!m_taskVector.empty()) {
        auto now = std::chrono::system_clock::now();
        for (auto& t : m_taskVector) {
            t.m_time = now + std::chrono::seconds(t.m_second);
            m_taskQ.push(t);
        }
    }
    while (m_running) {
        auto now = std::chrono::system_clock::now();
        if (!m_taskQ.empty()) {
            while(!m_taskQ.empty() && m_taskQ.top().m_time <= now) {
                unique_lock<mutex> lock{m_schedulerMutex, std::defer_lock};
                RepeatedTask t = m_taskQ.top();
                sensor::SensorDataMap temp;
                t.fetch(t.m_name, temp);
                utils::Log(utils::INFO, "RepeatedTask, fetching data from sensor:%s", t.m_name.c_str());
                lock.lock();
                m_taskQ.pop();
                now = std::chrono::system_clock::now(); 
                t.m_time = now +  std::chrono::seconds(t.m_second);
                m_taskQ.push(t);
                lock.unlock();
            }
            this_thread::sleep_for(m_taskQ.top().m_time - now);
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

void DeviceManager::pushEventToDevice(shared_ptr<utils::Event> evt) {
    lock_guard<mutex> lock(m_mutex);
    if (m_running) {
        m_queue.push(evt);
        m_condition.notify_one();    
        utils::Log(utils::INFO, "Post an event...");
    }
    else {
        utils::Log(utils::ERROR, "Device: %s is not running...", m_name.c_str());
    }
}

void DeviceManager::registerPollingTask(const string& sensorName, FetchFunc& func, int second) {
    utils::Log(utils::INFO, "Register a task...");
    if (m_running) {
        lock_guard<mutex> lock(m_schedulerMutex);
        auto time = std::chrono::system_clock::now() + std::chrono::seconds(second);
        m_taskQ.emplace(sensorName, func, second, time);
    }
    else {
        m_taskVector.emplace_back(sensorName, func, second);
    }
}


} //end of namespace device