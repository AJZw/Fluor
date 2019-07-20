/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-03-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_controller.h is part of Fluor
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
** The fluorophore menu controller
** 
** :class: Controller
** Builds and controlls the fluorophore menu. Combines a FLuorLineEdit with 
** a QScrollWidget and the fluorophore buttons
** Internally, connects the signal and slots of internal events and forms an
** interface for external signals and slots
**
***************************************************************************/

#ifndef FLUOR_CONTROLLER_H
#define FLUOR_CONTROLLER_H

#include <QWidget>
#include <QStringList>
#include <QPaintEvent>
#include <set>

#include <QScrollArea>

#include "cache.h"
#include "data_fluorophores.h"
#include "fluor_buttons.h"

namespace Fluor {

class Controller : public QWidget{
    Q_OBJECT
    
    public:
        explicit Controller(QWidget *parent = nullptr);
        Controller(const Controller &obj) = delete;
        Controller& operator=(const Controller &obj) = delete;
        Controller(Controller&&) = delete;
        Controller& operator=(Controller&&) = delete;
        ~Controller() = default;

        void paintEvent(QPaintEvent* event);

    public slots:
        void receiveGlobalEvent(QEvent* event);
        void receiveGlobalSize(const QWidget* widget=nullptr);
        void receiveData(const Data::Fluorophores& data);

        void receiveCacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void receiveCacheRemove(std::set<Data::FluorophoreID>& fluorophores);
        void receiveSync(const std::vector<Cache::CacheID>& input);

    private slots:
        void clickedPushButton(bool checked);
        void finishedLineEdit();
    
    signals:
        void sendGlobalEvent(QEvent* event);
        void sendGlobalSize(const QWidget* widget=nullptr);
        void sendData(const Data::Fluorophores& data);

        void showPushButton();
        void hidePushButton();
        void showLineEdit();
        void hideLineEdit();

        void sendCacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void sendCacheRemove(std::set<Data::FluorophoreID>& fluorophores);
        void sendSync(const std::vector<Cache::CacheID>& input);
};

class ButtonsController : public QWidget {
    Q_OBJECT

    public:
        explicit ButtonsController(QWidget* parent=nullptr);
        ButtonsController(const ButtonsController &obj) = delete;
        ButtonsController& operator=(const ButtonsController &obj) = delete;
        ButtonsController(ButtonsController&&) = delete;
        ButtonsController& operator=(ButtonsController&&) = delete;
        ~ButtonsController() = default;

        void paintEvent(QPaintEvent* event);

        void syncButtons(const Cache::CacheID& input);

    private:
        Fluor::EmissionButton* widget_emission;
        Fluor::ExcitationButton* widget_excitation;
        Fluor::RemoveButton* widget_remove;

        QString id;
        QString name;
        Data::CacheSpectrum* data;
    
    signals:
        void sendRemove(std::set<Data::FluorophoreID>& fluorophores);

    public slots:
        void receiveEmissionClick(bool active);
        void receiveExcitationClick(bool active);
        void receiveRemoveClick();
};

class ScrollController : public QScrollArea {
    Q_OBJECT

    public:
        explicit ScrollController(QWidget* parent=nullptr);
        ScrollController(const ScrollController &obj) = delete;
        ScrollController& operator=(const ScrollController &obj) = delete;
        ScrollController(ScrollController&&) = delete;
        ScrollController& operator=(ScrollController&&) = delete;
        ~ScrollController() = default;

    private:
        std::vector<ButtonsController*> button_widgets;

    signals:
        void sendRemove(std::set<Data::FluorophoreID>& fluorophores);

    public slots:
        void syncButtons(const std::vector<Cache::CacheID>& inputs);
        void receiveRemove(std::set<Data::FluorophoreID>& fluorophores);

};

} // Fluor namespace

#endif //FLUOR_CONTROLLER_H