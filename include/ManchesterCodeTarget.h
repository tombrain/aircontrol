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

#include <memory>
#include <string>

#include "Configuration.h"
#include "TargetParameters.h"
#include "Task.h"

/// Class responsible for target control.
class ManchesterCodeTarget : public Task {
public:
    /// Class constructor.
    ManchesterCodeTarget(Configuration & configuration, const std::string & manchestercode);

    /// Start the target control.
    int start(void) final;

private:
    /// Target section name.
    const std::string name_;

    /// Target parameters.
    std::unique_ptr<TargetParameters> parameters_;

    /// the manchestercode to send.
    const std::string manchestercode_;

    /// Control the target.
    void airControl(void) const;

    /// Send the air command with Manchester encoding.
    void sendAirCommandManchester(void) const;
};
