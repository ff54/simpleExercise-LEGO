# An application or API for fetching device sensor telemetry events.
This is a small application written in C++17, it has following modules
- DeviceManager: listen and fetch telemetry events from sensor.
- SensorSimulator: simulats sensor which generate data and posts events.
- utils: some help functions.
- demo.cc is a demo implementation.

## How to compile and run the demo app
_make_ compiles the applicaion and output the binary file DemoApp <br />
_./DemoApp_ runs the demo app. DemoApp has a simple terminal, type _help_ at the terminal for more informaton when you start the DemoApp
