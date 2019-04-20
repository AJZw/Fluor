/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-03-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_scrollarea.h is part of Fluor
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
** The Fluorophore menu scroll area
**
** :class: ScrollArea
** A ScrollArea for showing buttons
**
***************************************************************************/

#ifndef FLUOR_SCROLLAREA_H
#define FLUOR_SCROLLAREA_H

#include <QScrollArea>

namespace Fluor {

class ScrollArea : public QScrollArea {
    Q_OBJECT

    public:
        explicit ScrollArea(QWidget* parent=nullptr);
        ScrollArea(const ScrollArea &obj) = delete;
        ScrollArea& operator=(const ScrollArea &obj) = delete;
        ScrollArea(ScrollArea&&) = delete;
        ScrollArea& operator=(ScrollArea&&) = delete;
        ~ScrollArea() = default;
};

} // Fluor namespace

#endif //FLUOR_SCROLLAREA_H