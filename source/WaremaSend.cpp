/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014-2019 Ralf Dauberschmidt <ralf@dauberschmidt.de>
 *
 * aircontrol is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * aircontrol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with aircontrol.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <wiringPi.h>
#include <vector>
#include <sstream>

#include "WaremaSend.h"
namespace {
    std::string to_lower(const std::string & in)
    {
       std::string t { in };
        std::transform(t.begin(), t.end(), t.begin(),
            [](unsigned char c){ return std::tolower(c); });
        return t;
    }
    
    std::vector<std::string> split(const std::string &s, char delimiter) {     
        std::vector<std::string> tokens;     
        std::string token;     
        std::istringstream tokenStream(s);     
        while (getline(tokenStream, token, delimiter)) {      
            tokens.push_back(token);     
        }     
        return tokens;  
    }
}

/**
 * @param configuration Reference of the configuration.
 * @param name Target name string, must match a target configuration entry.
 */
WaremaSend::WaremaSend(Configuration & configuration, const std::string & deviceandaction) :
        Task(configuration),
        parameters_(nullptr),
        deviceandaction_(deviceandaction) {
}

/// @return Program exit code.
int WaremaSend::start(void) {
    auto p = split(deviceandaction_, '.');
    if (p.size() != 2)
    {
        std::cerr << "Error: param deviceandaction: " << deviceandaction_ << std::endl;
        return EXIT_FAILURE;
    }

    devicename_ = p[0];
    action_ = to_lower(p[1]); 
    
    std::cout << "Device: " << devicename_ << ", action: " << action_ << std::endl;
    // Check whether the device exists
    std::string sectionname { devicename_ };
    sectionname = "waremasend_" + devicename_;

    if (!configuration_.isValidSection(sectionname)) {
        std::cerr << "Error: Section (" << sectionname << ") for waremadevice " << devicename_ << " cannot be found"
            << std::endl;
        return EXIT_FAILURE;
    }
    assert(parameters_ == nullptr);
    parameters_ = std::make_unique<WaremaSendParameters>(
        WaremaSendParameters(configuration_, devicename_));

    // Load all parameters from the configuration
    if (!parameters_->load()) {
        return EXIT_FAILURE;
    }

    // Get GPIO from the parameters unless overridden from the command line
    if (gpioPin_ == Types::INVALID_GPIO_PIN) {
        gpioPin_ = parameters_->getGpioPin();
    } else if (!isValidGpioPin(gpioPin_)) {
        std::cerr << "Error: Given GPIO pin " << +gpioPin_ << " is invalid"
            << std::endl;
        return EXIT_FAILURE;
    }

    if (action_ == "up")
        manchestercode_ = parameters_->getManchesterCodeUp();
    else if (action_ == "down")
        manchestercode_ = parameters_->getManchesterCodeDown();
    else if (action_ == "stop")
        manchestercode_ = parameters_->getManchesterCodeStop();
    else
    {
        std::cerr << "Error: Action " << action_ << " is invalid" << std::endl;
        return EXIT_FAILURE;     
    }

    manchestercode_ = manchestercode_ + parameters_->getManchesterCodeDevice() + "S";

    std::cout << "Manchestercode to send: " << manchestercode_ << std::endl;
    
    // Send the radio frame to control the target
    airControl();

    std::cout << "successfull" << std::endl;
    return EXIT_SUCCESS;
}

void WaremaSend::airControl(void) const {

    pinMode(gpioPin_, OUTPUT);

    for (auto n = 0; n < parameters_->getSendCommand(); n++) {
        
        sendAirCommand();

        if (n != parameters_->getSendCommand() - 1) {
            digitalWrite(gpioPin_, LOW);
            usleep(parameters_->getSendDelay());
        }
    }

    pinMode(gpioPin_, INPUT);
}


void WaremaSend::sendAirCommand(void) const {
    
    for (auto i = 0U; i < manchestercode_.length(); i++) {
        switch (manchestercode_.at(i)) {
            case 's':
                digitalWrite(gpioPin_, LOW);
                usleep(parameters_->getSyncLength());
                break;

            case 'S':
                digitalWrite(gpioPin_, HIGH);
                usleep(parameters_->getSyncLength());
                break;

            case '0':
                // Falling edge in the middle of the pulse
                digitalWrite(gpioPin_, HIGH);
                usleep(parameters_->getDataLength() / 2);
                digitalWrite(gpioPin_, LOW);
                usleep(parameters_->getDataLength() / 2);
                break;

            case '1':
                // Rising edge in the middle of the pulse
                digitalWrite(gpioPin_, LOW);
                usleep(parameters_->getDataLength() / 2);
                digitalWrite(gpioPin_, HIGH);
                usleep(parameters_->getDataLength() / 2);
                break;
        }
    }
}
