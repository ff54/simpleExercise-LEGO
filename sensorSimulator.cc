#include <mutex>
#include <thread>
#include <functional>
#include <memory>

#include "sensorSimulator.hh"
#include "deviceManager.hh"
using namespace std;
namespace sensor {

SensorEvent::SensorEvent(const std::string& name, sensor::SensorDataMap& input): Event(name, utils::Event::SENSORINFO), data(input) {

}

SensorEvent::SensorEvent(SensorEvent& rhs): Event(rhs), data(rhs.data) {}

SensorEvent::SensorEvent(SensorEvent&& other): Event(other), data(move(other.data)) {}

SensorSimulator::SensorSimulator(const std::string& name, shared_ptr<device::DeviceManager>& dm, int freq)
    :m_name(name), m_dm(dm), m_freq(freq) { }

void SensorSimulator::start() {
    if (m_dm->getStatus() == device::ON) {
        m_running = true;
        m_scheduler = thread([&](){run();});
        m_scheduler.detach();
        utils::Log(utils::DEBUG, "Sensor %s is running...", m_name.c_str());
    }
    else {
        utils::Log(utils::ERROR, "Failed to start sensor %s DeviceManager is not ON !", m_name.c_str());
    }
}

SensorSimulator::~SensorSimulator() {
    m_running = false;
    if (m_scheduler.joinable())
        m_scheduler.join();
}


void SensorSimulator::run() {
    while (m_running) {
        if (m_dm->getStatus() == device::ON) myJob();
        this_thread::sleep_for(chrono::seconds(m_freq));
    }
}

}//end of namespace sensor