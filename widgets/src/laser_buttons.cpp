/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include <QStringList>
#include "laser_buttons.h"

namespace Laser {

/*
Initializer: builds a PushButton
*/
PushButton::PushButton(QWidget* parent) :
    QPushButton(parent)
{
    this->setText("Lasers");
    this->setToolTip("Add Laser");
}

/*
Slot: receives the selected lasers and update the text accordingly
*/
void PushButton::receiveGraphState(std::vector<State::GraphState>& state){
    for(const State::GraphState& graph_state : state){
        if(graph_state.isSelected()){
            if(graph_state.lasers().empty()){
                this->setText("Lasers");
            }else{
                // Construct text
                QStringList text_lasers = QStringList();

                for(const Data::Laser& laser : graph_state.lasers()){
                    QString text = "%1nm";
                    text = text.arg(
                        static_cast<int>(laser.wavelength())
                    );
                    text_lasers.append(text);
                }
                this->setText(text_lasers.join(", "));
            }
            return;
        }
    }  
    this->setText("Lasers");
}

/*
Slot: hides button
*/
void PushButton::hideButton(){
    this->hide();
}

/*
Slot: shows button
*/
void PushButton::showButton(){
    this->show();
}

} // Laser namespace
