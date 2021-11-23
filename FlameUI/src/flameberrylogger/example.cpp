#include "fby_logger.h"

int main()
{
    fby_logger::log("This is a really simple logger... for now!");

    std::string name;
    std::cout << "Enter your name: " << std::endl;
    std::cin >> name;

    fby_logger::log("Hello, {0}", name);
    fby_logger::log("I can log things like {0}, {1}, {2}, {3}, {4}, {5}", 105, 123.232343, 0.0005, 'a', "welcome", std::string("A String"));
}