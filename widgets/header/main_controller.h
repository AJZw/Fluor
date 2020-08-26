/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** maun_controller.h is part of Fluor
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
** Controller of the main widget
**
** :class: Main::Controller
** Controls the signal and slots of the main widgets
** 
***************************************************************************/

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <QMainWindow>
#include "global.h"
#include "data_fluorophores.h"
#include "data_instruments.h"
#include "cache.h"
#include "state_gui.h"
#include <vector>

#include <QScreen>
#include <QShowEvent>
#include <QHideEvent>

namespace Main {

class Controller : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(QString layout_margins READ layoutMargins WRITE setLayoutMargins)

    public:
        explicit Controller(QWidget *parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;

        QString layoutMargins() const;
        void setLayoutMargins(QString layout_spacing);

    private:
        QString window_title;
        QScreen* window_screen;
        int window_width;
        int window_height;

        bool eventFilter(QObject *obj, QEvent *event);

    public slots:
        virtual void show();
        virtual void hide();

        void receiveScreenChanged(QScreen* screen);
        void receiveDPIChanged(qreal dpi);

        void receiveGlobalEvent(QEvent* event);
        void receiveData(const Data::FluorophoreReader& data);
        void receiveInstrument(const Data::Instrument& instrument);

        void receiveCacheRequestUpdate();
        void receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
        void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
        void receiveCacheState(const std::vector<Cache::ID>& cache_state);
        void receiveCacheUpdate();

        void receiveLaser(int wavelength);

        void receiveToolbarStateChange(Bar::ButtonType type, bool active, bool enable);
        void receiveToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable);

        void receiveGraphSelect(std::size_t index, bool state);
        void receiveGraphState(std::vector<State::GraphState>& state);

    signals:
        void resized(const QWidget* widget);
        void moved(const QWidget* widget);
        void screenChanged(QWidget* widget);
        void screenDPIChanged(QWidget* widget);

        void sendGlobalEvent(QEvent* event);
        void sendData(const Data::FluorophoreReader& data);
        void sendInstrument(const Data::Instrument& instrument);

        void sendCacheRequestUpdate();
        void sendCacheAdd(std::vector<Data::FluorophoreID>& fluorophores);
        void sendCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);
        void sendCacheState(const std::vector<Cache::ID>& cache_state);
        void sendCacheUpdate();

        void sendLaser(int wavelength);

        void sendToolbarStateChange(Bar::ButtonType type, bool active, bool enable=true);
        void sendToolbarStateUpdate(Bar::ButtonType type, bool active, bool enable=true);
        
        void sendGraphSelect(std::size_t index, bool state);
        void sendGraphState(std::vector<State::GraphState>& state);
};

} // Main namespace

#endif //MAIN_CONTROLLER_H