#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;

class SerialCommunication {
private:
    io_service io;
    serial_port serial;

public:
    SerialCommunication(const std::string& port, unsigned int baud_rate)
        : serial(io, port) {
        serial.set_option(serial_port_base::baud_rate(baud_rate));
        serial.set_option(serial_port_base::character_size(8));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    }

    // Write data to serial port
    void writeData(const std::string& data) {
        boost::asio::write(serial, boost::asio::buffer(data.c_str(), data.size()));
    }

    // Read data from serial port
    std::string readData() {
        char buf[256];
        size_t len = serial.read_some(boost::asio::buffer(buf));
        return std::string(buf, len);
    }

    // Close the serial port
    void close() {
        if (serial.is_open()) {
            serial.close();
        }
    }

    // Check if port is open
    bool isOpen() {
        return serial.is_open();
    }
};

int main() {
    try {
        // Replace "COM3" with your actual port name
        // Common names are "COM3" for Windows, "/dev/ttyUSB0" for Linux
        SerialCommunication serial("COM3", 9600);

        if (serial.isOpen()) {
            std::cout << "Serial port opened successfully!" << std::endl;

            // Example: Write data
            std::string message = "Hello Serial Port!";
            serial.writeData(message);
            std::cout << "Sent: " << message << std::endl;

            // Example: Read data
            std::cout << "Waiting for response..." << std::endl;
            std::string response = serial.readData();
            std::cout << "Received: " << response << std::endl;

            serial.close();
        }
    }
    catch (boost::system::system_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}