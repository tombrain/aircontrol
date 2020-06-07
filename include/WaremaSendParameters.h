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

#pragma once

#include <string>

#include "Configuration.h"
#include "Types.h"

constexpr auto waremasend { "waremasend" };

/// Class holding all parameters required for WaremaSend tasks.
class WaremaSendParameters {
public:
    /// Class constructor.
    WaremaSendParameters(const Configuration & configuration,
        const std::string & devicename);

    /**
     * @brief Load all required configuration parameters.
     * @note Must be called before any of the getters.
     */
    bool load(void);

    /// Get the GPIO pin.
    uint8_t getGpioPin(void) const;

    /**
     * @brief Get the pulse length of a single air command data element.
     * @note Unit: microseconds
     */
    int32_t getDataLength(void) const;

    /**
     * @brief Get the pulse length of a single air command sync element.
     * @note Unit: microseconds
     */
    int32_t getSyncLength(void) const;

    /// Get m.code of device.
    std::string getManchesterCodeDevice(void) const;
    /// Get m.code of device.
    std::string getManchesterCodeDown(void) const;
    /// Get m.code of device.
    std::string getManchesterCodeUp(void) const;
    /// Get m.code of device.
    std::string getManchesterCodeStop(void) const;

    /// Get the number of times the air command will be transmitted.
    int32_t getSendCommand(void) const;

    /**
     * @brief Get the delay between repeated air command transmissions.
     * @note Unit: microseconds
     */
    int32_t getSendDelay(void) const;

private:
    /// Reference of the related configuration instance.
    const Configuration & configuration_;

    const std::string devicename_;

    /// GPIO pin.
    uint8_t gpioPin_;

    /**
     * @brief Pulse length of a single air command data element (0 or 1).
     * @note Unit: microseconds
     */
    int32_t dataLengthUs_;

    /**
     * @brief Pulse length of a single air command sync element (s or S).
     * @note Unit: microseconds
     */
    int32_t syncLengthUs_;

    /// device string.
    std::string manchesterCodeDevice_;

    /// action string.
    std::string manchesterCodeDown_;
    /// action string.
    std::string manchesterCodeUp_;
    /// action string.
    std::string manchesterCodeStop_;

    /// Number of times the air command will be transmitted.
    int32_t sendCommand_;

    /**
     * @brief Delay between repeated air command transmissions.
     * @note Unit: microseconds
     */
    int32_t sendDelayUs_;

    /**
     * @brief Get the requested configuration value from either the given
     *        section or the "waremasend" section.
     * @copydoc Configuration::getValue()
     */
    template <typename T>
    bool getValue(const std::string section, const std::string name,
            T & value) const {
        // combinend name of waremasend-devices
        std::string tmp { waremasend };
        tmp = tmp + "_" + devicename_;
        if (!configuration_.getValue(tmp, name, value)) {
            if (!configuration_.getValue(waremasend, name, value)) {
                std::cerr << "Error: Missing configuration parameter '" << name
                    << "'" << std::endl;
                return false;
            }
        }

        return true;
    }

    /// Load the GPIO pin from the configuration.
    bool loadGpioPin(void);

    /// Load the data length parameter from the configuration.
    bool loadDataLength(void);

    /// Load the sync length parameter from the configuration.
    bool loadSyncLength(void);

    /// 
    bool loadManchesterCodeDevice(void) {
        return loadManchestercode("manchesterCodeDevice", manchesterCodeDevice_);
    }

    /// 
    bool loadManchesterCodeDown(void) {
        return loadManchestercode("manchesterCodeDown", manchesterCodeDown_);
    }

    /// 
    bool loadManchesterCodeUp(void) {
        return loadManchestercode("manchesterCodeUp", manchesterCodeUp_);
    }

    /// 
    bool loadManchesterCodeStop(void) {
        return loadManchestercode("manchesterCodeStop", manchesterCodeStop_);
    }

    /// Load the send command parameter from the configuration.
    bool loadSendCommand(void);

    /// Load the send delay parameter from the configuration.
    bool loadSendDelay(void);

private:
    bool loadManchestercode(const std::string & manchestercode, std::string & var);
};
