/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_buttons.h is part of Fluor
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
** The Fluorophore menu buttons
**
** :class: PushButton
** A pushbutton to be shown if the FluorLineEdit is inactive
**
***************************************************************************/

#ifndef FLUOR_BUTTONS_H
#define FLUOR_BUTTONS_H

#include <QPushButton>

namespace Fluor {

class PushButton : public QPushButton {
    Q_OBJECT

    public:
        explicit PushButton(QWidget* parent=nullptr);
        PushButton(const PushButton &obj) = delete;
        PushButton& operator=(const PushButton &obj) = delete;
        PushButton(PushButton&&) = delete;
        PushButton& operator=(PushButton&&) = delete;
        ~PushButton() = default;
    
    public slots:
        void hideButton();
        void showButton();
};

} // Fluor namespace

#endif //FLUOR_BUTTONS_H