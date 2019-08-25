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
#include <set>
#include "data_fluorophores.h"
#include "cache.h"

namespace Main {

class Controller : public QMainWindow {
    Q_OBJECT

    public:
        explicit Controller(QWidget *parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;

    private:
        QString window_title;
        int window_width;
        int window_height;

        bool eventFilter(QObject *obj, QEvent *event);
    
    public slots:
        void receiveGlobalEvent(QEvent* event);

        void receiveLaser(int wavelength);

        void receiveCacheRequestUpdate();
        void receiveCacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);

        void receiveCacheSync(const std::vector<Cache::CacheID>& cache_state);
        void receiveCacheUpdate(const std::vector<Cache::CacheID>& cache_state);

    signals:
        void resized(const QWidget* widget);
        void moved(const QWidget* widget);

        void sendGlobalEvent(QEvent* event);
        void sendData(const Data::Fluorophores& data);

        void sendLaser(int wavelength);

        void sendCacheRequestUpdate();
        void sendCacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void sendCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);

        void sendCacheSync(const std::vector<Cache::CacheID>& cache_state);
        void sendCacheUpdate(const std::vector<Cache::CacheID>& cache_state);
};

} // Main namespace

#endif //MAIN_CONTROLLER_H