/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-22
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** test_data_fluorophores.cpp is part of Spectral Viewer Tests
**        
** Spectral Viewer is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** Spectral Viewer is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License along
** with Spectral Viewer. If not, see <https://www.gnu.org/licenses/>.  
***************************************************************************/

#include "catch.hpp"
#include "viewer_data_factory.h"
#include "viewer_data_fluorophores.h"

TEST_CASE("Data Fluorophores", "[DataFluorophores]"){
    // Check whether fluorophore.ini can be found and load fluorphore data
    DataFactory factory;
    REQUIRE_FALSE(factory.isWarning());

    DataFluorophores fluorophores;
    fluorophores.load(factory);

    std::vector<QString> fluor_name;                    // fluorophore names (for input list)
    std::unordered_map<QString, QString> fluor_id;      // fluorophore ID (for plotting)
    std::unordered_map<QString, QStringList> fluor_multiname; // alternative names
    std::unordered_map<QString, DataMeta> fluor_meta;   // meta data

    fluor_name = fluorophores.getFluorName();
    fluor_id = fluorophores.getFluorID();
    fluor_multiname = fluorophores.getFluorMultiname();
    fluor_meta = fluorophores.getFluorMeta();

    // Check if loading worked
    REQUIRE()


}

// check fluorophore loading
// check fallback {0,0}
