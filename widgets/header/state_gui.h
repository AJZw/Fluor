/**** General **************************************************************
** Version:    v0.10.1
** Date:       2020-11-16
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** state_gui.h is part of Fluor
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
** Stores the current GUI state (except for the spectra -> see cache)
**
** :class: State::GraphState
** GUI graph state
**
** :class: State::GUI
** Storage class for the global GUI properties
** 
***************************************************************************/

#ifndef STATE_GUI_H
#define STATE_GUI_H

#include "global.h"
#include "data_instruments.h"
#include <vector>

namespace State {

class GraphState {
    public:
        GraphState();
        GraphState(bool visible_lasers, bool visible_filters);
        GraphState(std::size_t size);
        GraphState(const GraphState&) = default;
        GraphState& operator=(const GraphState&) = default;
        GraphState(GraphState&&) = default;
        GraphState& operator=(GraphState&&) = default;
        ~GraphState() = default;

    private:
        std::vector<Data::Laser> graph_lasers;
        const Data::LaserLine* graph_laserline;
        bool selected;
        bool visible_lasers;
        bool visible_filters;
    
    public:
        bool isSelected() const;
        void setSelected(bool select);

        bool visibleLasers() const;
        void setVisibleLasers(bool visible);

        bool visibleFilters() const;
        void setVisibleFilters(bool visible);

        const std::vector<Data::Laser>& lasers() const;
        std::vector<Data::Laser>& setLasers();

        const Data::LaserLine* laserLine() const;
        void setLaserLine(const Data::LaserLine* laserline);
};

class GUI {
    public:
        GUI();
        GUI(const GUI&) = delete;
        GUI& operator=(const GUI&) = delete;
        GUI(GUI&&) = delete;
        GUI& operator=(GUI&&) = delete;
        ~GUI() = default;

    public:
        // ToolBar States
        bool active_laser = true;
        bool active_excitation = false;
        bool active_emission = true;
        bool active_filter = true;
        bool active_graph_add = true;
        bool active_graph_remove = false;
        bool active_laserlines = false;

        bool enabled_laser = true;
        bool enabled_filter = false;
        bool enabled_laserlines = false;

        // Global States
        QString style = QString("DARKPLUS");

        // Fluorophore properties
        SortMode sort_fluorophores = SortMode::Additive;

        // Graph States and properties
        ExcitationOption graph_option = ExcitationOption::SinglePhoton;
        std::size_t graph_count_max = 10;

    private:
        std::vector<GraphState> graphs_state;

    public:
        std::vector<GraphState>& graphs();
        std::size_t graphCount() const;
        void addGraph();
        void removeGraph();
        void clearGraphs();
        void fillGraphs(Data::Instrument& instrument);

        void addLasers(std::vector<Data::LaserID> lasers, Data::Instrument& instrument);
        void removeLasers();

        void setGraphVisibleLaser(bool visible);
        void setGraphVisibleFilter(bool visible);
        void setGraphSelect(std::size_t index, bool state);

    private:
        std::size_t findSelectedGraph() const;

};

} // State namespace

#endif // STATE_GUI_H
