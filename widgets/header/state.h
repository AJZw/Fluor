/**** General **************************************************************
** Version:    v0.9.5
** Date:       2019-09-12
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** state.h is part of Fluor
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
** :enum: State::ExcitationOption
** The fluoresence excitation method
**
** :struct: State::GUIGlobal
** Storage class for the global GUI properties
**
** :class: State::State
** Main state of the program. Combines GUI with the non-GUI properties
** 
***************************************************************************/

#ifndef STATE_H
#define STATE_H

#include <QObject>
#include "global.h"
#include "data_factory.h"
#include "data_fluorophores.h"
#include "data_instruments.h"
#include "data_styles.h"
#include "cache.h"
#include "main_controller.h"
#include "toolbar_controller.h"


namespace State {

enum class ExcitationOption {
    SinglePhoton,
    MultiPhoton
};

struct GUIGlobal {
    bool active_laser = false;
    bool active_excitation = false;
    bool active_emission = true;
    bool active_detector = false;
    bool active_graph_add = true;
    bool active_graph_remove = false;
    bool active_lasers = false;

    bool enabled_laser = false;
    bool enabled_detector = false;
    bool enabled_lasers = false;

    SortMode sort_fluorophores = SortMode::Additive;

    QString style = QString("BLACKBLUE");
};

class State : public QObject {
    Q_OBJECT
    
    public:
        State(Data::Factory& factory);
        State(const State&) = delete;
        State& operator=(const State&) = delete;
        State(State&&) = delete;
        State& operator=(State&&) = delete;
        ~State() = default;

    private:
        Data::Factory& factory;
        Data::FluorophoreReader data_fluorophores;
        Data::InstrumentReader data_instruments;
        Data::Style::Builder style;
        Cache::Cache cache;
        Main::Controller gui;

        GUIGlobal global;
        Data::Instrument instrument;

        void syncGlobal();
        void syncInstrument();
        void syncToolbar();
        void syncCache();

    signals:
        void sendToolbarState(Bar::ButtonType type, bool active, bool enable);
        void sendCacheState(Cache::CacheState state);
        void sendCacheStateExcitation(bool visible);
        void sendCacheStateEmission(bool visible);
        void sendCacheStateSorting(SortMode mode);

    public slots:
        void receiveToolbarState(Bar::ButtonType type, bool active, bool enable);

    private slots:
        void reloadStyle(QWidget* source);

};


}   // State namespace

#endif  // STATE_H
