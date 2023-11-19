#include "deviceManager.hh"
#include <iostream>
#include <future>
namespace device {
using namespace std;

Event::Event(sensor::SensorDataMap& input, const string& name): timeStamp(chrono::system_clock::now()), data(input),
                                            sensorName(name) {
}
Event::Event(Event& rhs):timeStamp(rhs.timeStamp), data(rhs.data), sensorName(rhs.sensorName){
}


Event::Event(const string& name): timeStamp(chrono::system_clock::now()), sensorName(name){
}

Event::Event(Event&& other): timeStamp(move(other.timeStamp)), data(move(other.data)) {
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
    if (m_running) {
        return;
    }
    try{
        m_running = true;
        m_thread = thread([&](){run();});
    }
    catch (exception& e) {
        cout<<"caught an exception: "<<e.what()<<endl;
    }
    m_thread.detach();
    m_status = ON;
}

void DeviceManager::stop() {
    unique_lock<mutex> lock(m_mutex);
    m_running = false;
    lock.unlock();
    m_condition.notify_one();    
    cout<< "finishing..."<<endl;
    queue<Event>().swap(m_queue);
    priority_queue<RepeatedTask>().swap(m_taskQ);
    m_status = OFF;
}

void DeviceManager::run() {
    cout<< "Running"<<endl;
    while (m_running)
    {
        unique_lock<mutex> lock(m_mutex);
        cout<< "ongoing"<<endl;
        m_condition.wait(lock, [this](){ return (!m_queue.empty() || !m_running);});
        if(!m_running) {
            return;
        }
        Event& event = m_queue.front();
        if (!event.data.empty()) {
            time_t t = chrono::system_clock::to_time_t(event.timeStamp);
            auto res = m_cache.emplace(event.sensorName, make_pair(t, event.data));
            if (!res.second) {
                res.first->second.first = t;
                for (auto& p : event.data) {
                    res.first->second.second[p.first] = p.second;
                }
            }
            cout<<"consume an event"<<endl;
        }
        m_queue.pop();
        lock.unlock();

    }
}

void DeviceManager::PushDeviceEvent(Event evt) {
    lock_guard<mutex> lock(m_mutex);
    if (m_running) {
        m_queue.push(move(evt));
        m_condition.notify_one();    
        cout<<"post an event: "<<m_queue.size()<<endl;
    }
}


class RepeatedTask
{
public:
    RepeatedTask(function<void(const string&, sensor::SensorDataMap&)>& fetchFunc, int second): m_func(move(fetchFunc)) { }
    bool operator<(const RepeatedTask& rhs) const {
        return m_time > rhs.m_time;
    }

    void fetch(const string& sensorName, sensor::SensorDataMap& data) { m_func(sensorName, data); }
private:
    function<void(string, sensor::SensorDataMap&)> m_func;
    chrono::system_clock::time_point m_time;
};

} //end of namespace device