/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** graph_settings.h is part of Fluor
**        
** Fluor is free software: you can redistribute it and/or
** modify it under the terms of the Lesser GNU General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
** 
** Fluor is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Lesser
** GNU General Public License for more details.
** 
** You should have received a copy of the Lesser GNU General Public License
** along with Fluor. If not, see <https://www.gnu.org/licenses/>.  
***************************************************************************/

/**** DOC ******************************************************************
**
** 
***************************************************************************/

#ifndef GRAPH_SETTINGS_H
#define GRAPH_SETTINGS_H

#include <QString>
#include <array>

namespace Graph {

struct Axis {
    explicit Axis(int min, int max);
    Axis(const Axis &obj) = default;
    Axis& operator=(const Axis &obj) = default;
    Axis(Axis&&) = default;
    Axis& operator=(Axis&&) = default;
    ~Axis() = default;

    const int min;
    const int max;
};

struct AxisRange {
    explicit AxisRange(int begin, int end);
    AxisRange(const AxisRange &obj) = default;
    AxisRange& operator=(const AxisRange &obj) = default;
    AxisRange(AxisRange&&) = default;
    AxisRange& operator=(AxisRange&&) = default;
    ~AxisRange() = default;

    int begin;
    int end;
    const int default_begin;
    const int default_end;
};

struct Tick {
    explicit Tick(int location);
    explicit Tick(int location, QString label);
    Tick(const Tick &obj) = default;
    Tick& operator=(const Tick &obj) = default;
    Tick(Tick&&) = default;
    Tick& operator=(Tick&&) = default;
    ~Tick() = default;

    const int location;
    const QString label;
};

template<std::size_t TICK_COUNT>
struct Ticks {
    Ticks(std::array<Tick, TICK_COUNT> const& ticks)  :
        valid(false), index_begin(0), index_end(0),
        ticks(ticks)
    {};
    Ticks(const Ticks &obj) = default;
    Ticks& operator=(const Ticks &obj) = default;
    Ticks(Ticks&&) = default;
    Ticks& operator=(Ticks&&) = default;
    ~Ticks() = default;

    bool valid;
    std::size_t index_begin;
    std::size_t index_end;      // Technically the index after the last relevant tick index, so can be out-of-bounds
    const std::array<Tick, TICK_COUNT> ticks;

    void findIndexes(int begin, int end);
};

struct Settings {
    Settings();
    Settings(const Settings &obj) = default;
    Settings& operator=(const Settings &obj) = default;
    Settings(Settings&&) = default;
    Settings& operator=(Settings&&) = default;
    ~Settings() = default;

    const QString x_title = "Wavelength (nm)";
    const Axis x_axis = Axis(0, 1500);
    AxisRange x_range = AxisRange(300, 900);
    Ticks<31> x_ticks = {{
        Tick(0, "0"), Tick(50),
        Tick(100, "100"), Tick(150), 
        Tick(200, "200"), Tick(250), 
        Tick(300, "300"), Tick(350), 
        Tick(400, "400"), Tick(450), 
        Tick(500, "500"), Tick(550), 
        Tick(600, "600"), Tick(650), 
        Tick(700, "700"), Tick(750), 
        Tick(800, "800"), Tick(850), 
        Tick(900, "900"), Tick(950), 
        Tick(1000, "1000"), Tick(1050), 
        Tick(1100, "1100"), Tick(1150), 
        Tick(1200, "1200"), Tick(1250), 
        Tick(1300, "1300"), Tick(1350), 
        Tick(1400, "1400"), Tick(1450), 
        Tick(1500, "1500")
    }};

    const QString y_title = "Intensity (%)";
    const Axis y_axis = Axis(100, 0);
    AxisRange y_range = AxisRange(100, 0);
    Ticks<6> y_ticks = {{
        Tick(0, "0"),
        Tick(20, "20"),
        Tick(40, "40"), 
        Tick(60, "60"), 
        Tick(80, "80"), 
        Tick(100, "100")
    }};
};

} // Graph namespace

#endif // GRAPH_SETTINGS_H