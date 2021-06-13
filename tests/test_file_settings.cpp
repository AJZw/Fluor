/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-22
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** test_file_settings.cpp is part of Spectral Viewer Tests
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

TEST_CASE("File Settings", "[Settings.ini]"){
    // Make sure the ini data files exists
    DataFactory factory;
    REQUIRE(factory.isValid());

    // Work with 'RAW' QSettings, might want to move these functions into a class eventually
    std::unique_ptr<QSettings> settings;
    settings = factory.get("settings");

    // Test whether it contain one entree, named default
    QStringList settings_ids = settings->childGroups();

    CHECK(settings_ids.size() == 1);
    REQUIRE(settings_ids.contains("DEFAULT"));

    settings->beginGroup("DEFAULT");

    QStringList settings_keys = settings->childKeys();

    CHECK(settings_keys.size() == 7);
    CHECK(settings_keys.contains("width"));
    CHECK(settings_keys.contains("height"));
    CHECK(settings_keys.contains("cytometer"));
    CHECK(settings_keys.contains("style"));
    CHECK(settings_keys.contains("sort_option"));
    CHECK(settings_keys.contains("sort_reversed"));
    CHECK(settings_keys.contains("sort_presort"));

    settings->endGroup();
}