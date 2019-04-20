/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-01-17
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** data_global.h is part of Fluor
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
** General purpose header file
** > Builds import and export macro declaration
** > Adds hashing function for QString's
**
***************************************************************************/

#ifndef DATA_GLOBAL_H
#define DATA_GLOBAL_H

#include <QtCore/QtGlobal>
#include <QHash>

#if defined(DATALIB_LIBRARY)
    #define DATALIB_EXPORT Q_DECL_EXPORT
#else
    #define DATALIB_EXPORT Q_DECL_IMPORT
#endif

// Adds hashing function for QStrings
namespace std {
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}

#endif // DATA_GLOBAL_H