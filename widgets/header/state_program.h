/**** General **************************************************************
** Version:    v0.9.7
** Date:       2020-01-17
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** state_program.h is part of Fluor
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
** Handles the global state and connections of the GUI to non-GUI properties
** Aka the glue between all parts
**
** :class: State::Program
** Main state of the program. Combines GUI with the non-GUI properties
** 
***************************************************************************/

#ifndef STATE_PROGRAM_H
#define STATE_PROGRAM_H

#include <QObject>
#include "global.h"
#include "data_factory.h"
#include "data_fluorophores.h"
#include "data_instruments.h"
#include "data_styles.h"
#include "cache.h"
#include "state_gui.h"
#include "main_controller.h"

namespace State {

class Program : public QObject {
    Q_OBJECT
    
    public:
        Program(Data::Factory& factory);
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;
        Program(Program&&) = delete;
        Program& operator=(Program&&) = delete;
        ~Program() = default;

    private:
        Data::Factory& factory;
        Data::FluorophoreReader data_fluorophores;
        Data::InstrumentReader data_instruments;
        Data::Style::Builder style;
        Data::Instrument instrument;

        Cache::Cache cache;
        State::GUI state_gui;
        Main::Controller gui;

        void retreiveStateGUI();
        void retreiveInstrument();
    
        void syncToolbar();
        void syncGraphs();
        void syncCache();
        void syncInstrument();
        void syncFluorophores();

        void loadInstrument(const QString& instrument_name);
        void refreshToolbar();

    signals:
        void sendToolbarState(Bar::ButtonType type, bool active, bool enable);
        
        void sendCacheState(Cache::CacheState state);
        void sendCacheStateExcitation(bool visible);
        void sendCacheStateEmission(bool visible);
        void sendCacheStateSorting(SortMode mode);
        
        void sendGraphState(std::vector<State::GraphState>& state);

    public slots:
        void receiveToolbarState(Bar::ButtonType type, bool active, bool enable);
        void receiveLaser(double wavelength);
        void receiveLasers(std::vector<double>& wavelength);
        void receiveGraphSelect(std::size_t index, bool state);

    private slots:
        void reloadStyle(QWidget* source);

};


}   // State namespace

#endif  // STATE_PROGRAM_H
