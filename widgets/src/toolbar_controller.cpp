/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "toolbar_controller.h"
#include "toolbar_buttons.h"
#include <QHBoxLayout>
#include <QStyleOption>
#include <QStyle>
#include <QPainter>
#include <QDebug>

namespace Bar {

/*
Initializer: builds and connects the widgets of the quick options bar
    :param parent: parent widget
*/
Controller::Controller(QWidget* parent) :
    QWidget(parent)
{
    // Set base properties
    this->setContentsMargins(0, 0, 0, 0);

    // Build layout
    QHBoxLayout* controller_layout = new QHBoxLayout(this);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(controller_layout);

    Bar::LaserButton* widget_laser = new Bar::LaserButton(this);
    Bar::ExcitationButton* widget_excitation = new Bar::ExcitationButton(this);
    Bar::EmissionButton* widget_emission = new Bar::EmissionButton(this);
    Bar::DetectorButton* widget_detector = new Bar::DetectorButton(this);
    controller_layout->addWidget(widget_laser);
    controller_layout->addWidget(widget_excitation);
    controller_layout->addWidget(widget_emission);
    controller_layout->addWidget(widget_detector);

    controller_layout->addStretch(1);

    Bar::GraphAddButton* widget_graph_add = new Bar::GraphAddButton(this);
    Bar::GraphRemoveButton* widget_graph_remove = new Bar::GraphRemoveButton(this);
    controller_layout->addWidget(widget_graph_add);
    controller_layout->addWidget(widget_graph_remove);

    controller_layout->addStretch(1);

    //Bar::Label* widget_label = new Bar::Label(this);
    Bar::LasersButton* widget_lasers = new Bar::LasersButton(this);
    //controller_layout->addWidget(widget_label);
    controller_layout->addWidget(widget_lasers);

    // Connections
    QObject::connect(this, &Bar::Controller::enableLaser, widget_laser, Bar::LaserButton::setEnabled);
    QObject::connect(this, &Bar::Controller::enableDetector, widget_detector, Bar::DetectorButton::setEnabled);
    QObject::connect(this, &Bar::Controller::enableLasers, widget_lasers, &Bar::LasersButton::setEnabled);

    QObject::connect(widget_laser, &Bar::SquarePushButton::clicked, this, Bar::Controller::clickedLaser);
    QObject::connect(widget_excitation, &Bar::SquarePushButton::clicked, this, Bar::Controller::clickedExcitation);
    QObject::connect(widget_emission, &Bar::SquarePushButton::clicked, this, Bar::Controller::clickedEmission);
    QObject::connect(widget_detector, &Bar::SquarePushButton::clicked, this, Bar::Controller::clickedDetector);
    QObject::connect(widget_graph_add, &Bar::SquarePushButton::clicked, this, Bar::Controller::clickedGraphAdd);
    QObject::connect(widget_graph_remove, &Bar::SquarePushButton::clicked, this, Bar::Controller::clickedGraphRemove);
    QObject::connect(widget_lasers, &Bar::LasersButton::clicked, this, Bar::Controller::clickedLasers);

}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
    :param event: paintevent
*/
void Controller::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QStyleOption style_option;
    style_option.initFrom(this);
    QPainter painter(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Slot: handles widget_laser::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedLaser(bool active) {
    emit this->setVisibilityLaser(active);
}

/*
Slot: handles widget_excitation::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedExcitation(bool active) {
    emit this->setVisibilityExcitation(active);
}

/*
Slot: handles widget_emission::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedEmission(bool active) {
    emit this->setVisibilityEmission(active);
}

/*
Slot: handles widget_detector::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedDetector(bool active){
    emit this->setVisibilityDetector(active);
}

/*
Slot: handles widget_graph_add::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedGraphAdd(bool active) {
    emit this->addGraph(active);
}

/*
Slot: handles widget_graph_remove::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedGraphRemove(bool active) {
    emit this->removeGraph(active);
}

/*
Slot: handles widget_laserse::clicked() signals
    :param active: whether the widget is active or non-active (after the click)
*/
void Controller::clickedLasers(bool active) {
    emit this->expandLasers(active);
}

} // Bar namespace

/*
class BarLayout(QtWidgets.QHBoxLayout):

        # Hides cytometer specific buttons if there is no cytometer active
        if self.data.cytometers is None or self.data.cytometers.group() == "":
            self.add_graph_button.hide()
            self.delete_graph_button.hide()
            self.all_laser_button.hide()
            self.detector_button.hide()


    def resetSelf(self):
        """ Resets BarLayout & Indirectly the GraphPlot to 'new' state """
        self.toggleEmission(visible=self.graph.visible_emission)
        self.toggleExcitation(visible=self.graph.visible_excitation)
        self.toggleDetector(visible=self.graph.visible_detector)

        if self.data.cytometers is not None and self.data.cytometers.group() != "":
            self.graph_max = self.data.cytometers.value("laser_count", 1, type=int)
        else:
            self.graph_max = 1

        count = self.graph.graphCount()
        if count > self.graph_max:
            self.deleteGraphPlot(amount=count-self.graph_max)
            # self.deleteGraphPlot runs the toggleAddGraphPlot & toggleDeleteGraphPlot
        else:
            self.toggleAddGraphPlot(count)
            self.toggleDeleteGraphPlot(count)

        # Hides cytometer specific buttons if there is no cytometer active
        if self.data.cytometers is None or self.data.cytometers.group() == "":
            self.add_graph_button.hide()
            self.delete_graph_button.hide()
            self.all_laser_button.hide()
            self.detector_button.hide()
        else:
            self.add_graph_button.show()
            self.delete_graph_button.show()
            self.all_laser_button.show()
            self.detector_button.show()


*/