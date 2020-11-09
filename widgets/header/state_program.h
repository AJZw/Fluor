/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
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
        Data::StyleBuilder style;
        Data::Instrument instrument;

        Cache::Cache cache;
        State::GUI state_gui;
        Main::Controller gui;

        void retreiveGUIState();
        void retreiveGUIPosition();
        void retreiveInstrument();

        void storeStateGUI();
    
        void syncToolbar();
        void syncGraphs();
        void syncCache();
        void syncFluorophores();
        void syncInstrument();
        void syncInstruments();
        void syncStyle();
        void syncStyles();
        void syncOptions();

        void loadInstrument(const QString& instrument_id);
        void loadStyle(const QString& style_id);
        void refreshToolbar();

    signals:
        void sendFluorophores(const Data::FluorophoreReader& fluorophores);
        void sendInstrument(const Data::Instrument& instrument);
        void sendInstruments(const Data::InstrumentReader& instruments);
        void sendStyles(const std::vector<Data::StyleID>& styles);

        void sendMenuBarState(Main::MenuBarAction action, const QVariant& id);
        void sendToolbarState(Bar::ButtonType type, bool active, bool enable);
        
        void sendCacheState(const std::vector<Cache::ID>& cache_state);
        void sendCacheUpdate();
       
        void sendGraphState(std::vector<State::GraphState>& state);

    public slots:
        void receiveMenuBarState(Main::MenuBarAction action, const QVariant& id);
        void receiveToolbarState(Bar::ButtonType type, bool active, bool enable);

        void receiveLasers(std::vector<Data::LaserID>& lasers);
        
        void receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
        void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
        void receiveCacheRequestSync();
        void receiveCacheRequestUpdate();
        
        void receiveGraphSelect(std::size_t index, bool state);

    private slots:
        void closedWindow(const QWidget* source);
        void reloadStyle(QWidget* source);

};

}   // State namespace

#endif  // STATE_PROGRAM_H
