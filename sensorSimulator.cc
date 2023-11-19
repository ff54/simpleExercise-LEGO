#include "sensorSimulator.hh"

namespace sensor {

SensorEvent::SensorEvent(const std::string& name, sensor::SensorDataMap& input): Event(name, utils::Event::SENSORINFO), data(input) {

}

SensorEvent::SensorEvent(SensorEvent& rhs): Event(rhs), data(rhs.data) {}

SensorEvent::SensorEvent(SensorEvent&& other): Event(other), data(move(other.data)) {}

}//end of namespace sensor