# GPIO Breadboard Controller

## Wiring Diagram

This diagram shows how the Raspberry Pi GPIO pins are connected to the breadboard buttons.

![Wiring Diagram](wiring_diagram.png)

## Installation Instructions:

### Prerequisites:
sudo apt update

sudo apt install -y gcc build-essential wiringpi

sudo modprobe uinput

echo uinput | sudo tee -a /etc/modules


### Install:
git clone https://github.com/memphisb205/breadboard-controller.git

cd breadboard-controller/src

make

make install


### Clean Up Build Files:
make clean


### Uninstall:
make uninstall

## Roadmap 🗺️
- Analog joystick support for expanded game compatibility

- Configurable GPIO pin mapping via a config file

- LED activity indicator to show controller status



