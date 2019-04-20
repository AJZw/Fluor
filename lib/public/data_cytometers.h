/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License ******************************************************** 
** data_cytometers.h is part of Fluor
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
** 
** :class: Datacytometers
** Data container for cytometer information
** 
***************************************************************************/

#ifndef DATA_CYTOMETERS_H
#define DATA_CYTOMETERS_H

#include "data_global.h"
#include "data_factory.h"

#include <memory>

class DATALIB_EXPORT DataCytometers {
    public:
        DataCytometers();
        void load(DataFactory& data);
        void unload();
        bool isValid() const;

    private:
        bool data_loaded;

};

#endif //DATA_CYTOMETERS_H