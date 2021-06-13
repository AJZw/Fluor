/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-22
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** test_file_styles.cpp is part of Spectral Viewer Tests
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

TEST_CASE("File Styles", "[Settings.ini]"){
    // Make sure the ini data files exists
    DataFactory factory;
    REQUIRE_FALSE(factory.isWarning());

    // Work with 'RAW' QSettings, might want to move these functions into a class eventually
    std::unique_ptr<QSettings> styles;
    styles = factory.get("styles");

    QStringList style_ids = styles->childGroups();

    for(const QString& style_id : style_ids){
        styles->beginGroup(style_id);
        QStringList style_keys = styles->childKeys();

        // Check all minimum required keys for style formation 
        SECTION(style_id.toStdString()){
            CHECK(style_keys.contains("name"));
            CHECK(style_keys.contains("icons"));
            CHECK(style_keys.contains("main_background"));
            CHECK(style_keys.contains("widget"));
            CHECK(style_keys.contains("widget_inactive"));
            CHECK(style_keys.contains("text_color"));
            CHECK(style_keys.contains("text_color_disabled"));
            CHECK(style_keys.contains("text_weight"));
            CHECK(style_keys.contains("border_color"));
            CHECK(style_keys.contains("border_width"));
            CHECK(style_keys.contains("selection_color"));
            CHECK(style_keys.contains("selection_text_color"));
            CHECK(style_keys.contains("popup"));
            CHECK(style_keys.contains("popup_disabled"));
            CHECK(style_keys.contains("scrollbar_border_width"));
            CHECK(style_keys.contains("scrollbar_handle"));
            CHECK(style_keys.contains("scrollbar_background"));
            CHECK(style_keys.contains("fluormenu_remove"));
            CHECK(style_keys.contains("fluormenu_background"));
            CHECK(style_keys.contains("graph_plot_focus"));
            CHECK(style_keys.contains("graph_axis"));
            CHECK(style_keys.contains("graph_grid"));
            CHECK(style_keys.contains("graph_detector"));
        }

        styles->endGroup();
    }
}
