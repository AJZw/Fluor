/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** central_controller.h is part of Fluor
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
** Controller of central widget
**
** :class: Central::Controller
** Controls the central_widget layout and connects the signals and slots for
** the laser menu, toolbar buttons, fluorophore menu, and graphs
** 
***************************************************************************/

#ifndef CENTRAL_CONTROLLER_H
#define CENTRAL_CONTROLLER_H

#include <QWidget>
#include <QPaintEvent>
#include <set>
#include "data_fluorophores.h"
#include "cache.h"

namespace Central {

class Controller : public QWidget {
    Q_OBJECT

    public:
        explicit Controller(QWidget* parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;
        void paintEvent(QPaintEvent* event);

    public slots:
        void receiveGlobalEvent(QEvent* event);
        void receiveData(const Data::Fluorophores& data);
        void receiveGlobalSize(const QWidget* widget=nullptr);

        void receiveCacheRequestUpdate();
        void receiveCacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);

        void receiveCacheSync(const std::vector<Cache::CacheID>& cache_state);
        void receiveCacheUpdate(const std::vector<Cache::CacheID>& cache_state);

        void receiveLaser(int wavelength);

    signals:
        void sendGlobalEvent(QEvent* event); 
        void sendGlobalSize(const QWidget* widget=nullptr);
        void sendData(const Data::Fluorophores& data);

        void sendCacheRequestUpdate();
        void sendCacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void sendCacheRemove(std::vector<Data::FluorophoreID>& fluorophores);

        void sendCacheSync(const std::vector<Cache::CacheID>& cache_state);
        void sendCacheUpdate(const std::vector<Cache::CacheID>& cache_state);
        
        void sendLaser(int wavelength);

};

} // Central namespace

#endif //CENTRAL_CONTROLLER_H