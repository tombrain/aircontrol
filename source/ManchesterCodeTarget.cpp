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

#include <wiringPi.h>

#include "ManchesterCodeTarget.h"

/**
 * @param configuration Reference of the configuration.
 * @param name Target name string, must match a target configuration entry.
 */
ManchesterCodeTarget::ManchesterCodeTarget(Configuration & configuration, const std::string & manchestercode) :
        Task(configuration),
        name_("warema_sample"),
        parameters_(nullptr),
        manchestercode_(manchestercode) {
    // Do nothing
}

/// @return Program exit code.
int ManchesterCodeTarget::start(void) {
    std::cout << "Manchestercode to send: " << manchestercode_ << std::endl;
    // Check whether the target exists
    if (!configuration_.isValidSection(name_)) {
        std::cerr << "Error: Given target " << name_ << " cannot be found"
            << std::endl;
        return EXIT_FAILURE;
    }
    assert(parameters_ == nullptr);
    parameters_ = std::make_unique<TargetParameters>(
        TargetParameters(configuration_, name_));

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

    // Send the radio frame to control the target
    airControl();

    std::cout << "successfull" << std::endl;
    return EXIT_SUCCESS;
}

void ManchesterCodeTarget::airControl(void) const {
    void (ManchesterCodeTarget::*sendAirCommand)(void) const = nullptr;

    sendAirCommand = &ManchesterCodeTarget::sendAirCommandManchester;

    pinMode(gpioPin_, OUTPUT);

    for (auto n = 0; n < parameters_->getSendCommand(); n++) {
        (this->*sendAirCommand)();

        if (n != parameters_->getSendCommand() - 1) {
            digitalWrite(gpioPin_, LOW);
            usleep(parameters_->getSendDelay());
        }
    }

    pinMode(gpioPin_, INPUT);
}


void ManchesterCodeTarget::sendAirCommandManchester(void) const {
    assert(parameters_->getAirCode() == Types::AirCode::MANCHESTER);

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
