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

#include "WaremaSendParameters.h"
#include "Task.h"

/**
 * @param configuration Reference of the configuration.
 * @param name Reference of the related target name.
 */
WaremaSendParameters::WaremaSendParameters(const Configuration & configuration,
        const std::string & devicename) :
        configuration_(configuration),
        devicename_(devicename),
        gpioPin_(Types::INVALID_GPIO_PIN),
        dataLengthUs_(Types::INVALID_PARAMETER),
        syncLengthUs_(Types::INVALID_PARAMETER),
        manchesterCodeDevice_(),
        manchesterCodeDown_(),
        manchesterCodeUp_(),
        manchesterCodeStop_(),
        sendCommand_(Types::INVALID_PARAMETER),
        sendDelayUs_(Types::INVALID_PARAMETER) {
    // Do nothing
}

/// @return Status of the operation.
bool WaremaSendParameters::load(void) {
    return loadGpioPin()
        && loadDataLength()
        && loadSyncLength()
        && loadManchesterCodeDevice()
        && loadManchesterCodeDown()
        && loadManchesterCodeUp()
        && loadManchesterCodeStop()
        && loadSendCommand()
        && loadSendDelay();
}

/// @return GPIO pin.
uint8_t WaremaSendParameters::getGpioPin(void) const {
    assert(gpioPin_ != Types::INVALID_GPIO_PIN);
    return gpioPin_;
}

/// @return Pulse length of a single air command data element (0 or 1).
int32_t WaremaSendParameters::getDataLength(void) const {
    assert(dataLengthUs_ != Types::INVALID_PARAMETER);
    return dataLengthUs_;
}

/// @return Pulse length of a single air command sync element (s or S).
int32_t WaremaSendParameters::getSyncLength(void) const {
    assert(syncLengthUs_ != Types::INVALID_PARAMETER);
    return syncLengthUs_;
}

/// @return Sequence string manchesterCodeDown.
std::string WaremaSendParameters::getManchesterCodeDevice(void) const {
    assert(manchesterCodeDevice_.length() != 0U);
    return manchesterCodeDevice_;
}

/// @return Sequence string manchesterCodeDown.
std::string WaremaSendParameters::getManchesterCodeDown(void) const {
    assert(manchesterCodeDown_.length() != 0U);
    return manchesterCodeDown_;
}

/// @return Sequence string manchesterCodeUp.
std::string WaremaSendParameters::getManchesterCodeUp(void) const {
    assert(manchesterCodeUp_.length() != 0U);
    return manchesterCodeUp_;
}

/// @return Sequence string manchesterCodeStop.
std::string WaremaSendParameters::getManchesterCodeStop(void) const {
    assert(manchesterCodeStop_.length() != 0U);
    return manchesterCodeStop_;
}

/// @return Number of times the air command will be transmitted.
int32_t WaremaSendParameters::getSendCommand(void) const {
    assert(sendCommand_ != Types::INVALID_PARAMETER);
    return sendCommand_;
}

/// @return Delay between repeated air command transmissions.
int32_t WaremaSendParameters::getSendDelay(void) const {
    assert(sendDelayUs_ != Types::INVALID_PARAMETER);
    return sendDelayUs_;
}

/// @return True if successful, false otherwise.
bool WaremaSendParameters::loadGpioPin(void) {
    int32_t value;

    if (!getValue(devicename_, "gpioPin", value)) {
        return false;
    }

    if (!Task::isValidGpioPin(value)) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): gpioPin " << value << " is invalid" << std::endl;
        return false;
    }

    gpioPin_ = static_cast<uint8_t>(value);

    return true;
}

/// @return True if successful, false otherwise.
bool WaremaSendParameters::loadDataLength(void) {
    if (!getValue(devicename_, "dataLength", dataLengthUs_)) {
        return false;
    }

    if (dataLengthUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): dataLength is undefined" << std::endl;
        return false;
    } else if (dataLengthUs_ <= 0) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): dataLength is invalid" << std::endl;
        return false;
    }

    return true;
}

/// @return True if successful, false otherwise.
bool WaremaSendParameters::loadSyncLength(void) {
    if (!getValue(devicename_, "syncLength", syncLengthUs_)) {
        return false;
    }

    if (syncLengthUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): syncLength is undefined" << std::endl;
        return false;
    } else if (syncLengthUs_ < 0) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): syncLength is invalid" << std::endl;
        return false;
    }

    return true;
}



bool WaremaSendParameters::loadManchestercode(const std::string & manchestercode, std::string & var) {
    if (!getValue(devicename_, manchestercode, var)) {
        return false;
    }

    if (var.length() == 0U) {
        std::cerr << "Error: Configuration error (devicename " << devicename_
            << "): " << manchestercode << " is undefined" << std::endl;
        return false;
    } else {
        constexpr auto elements{"01sS"};

        const size_t position = var.find_first_not_of(elements);
        if (position != std::string::npos) {
            std::cerr << "Error: Configuration error (devicename_ " << devicename_
                << "): " << manchestercode  << " contains illegal character at position "
                << position+1 << std::endl;
            return false;
        }
    }

    return true;
}

/// @return True if successful, false otherwise.
bool WaremaSendParameters::loadSendCommand(void) {
    if (!getValue(devicename_, "sendCommand", sendCommand_)) {
        return false;
    }

    if (sendCommand_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): sendCommand is undefined" << std::endl;
        return false;
    } else if (sendCommand_ <= 0) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): sendCommand is invalid" << std::endl;
        return false;
    }

    return true;
}

/// @return True if successful, false otherwise.
bool WaremaSendParameters::loadSendDelay(void) {
    if (!getValue(devicename_, "sendDelay", sendDelayUs_)) {
        return false;
    }

    if (sendDelayUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (devicename_ " << devicename_
            << "): sendDelay is undefined" << std::endl;
        return false;
    }

    return true;
}
