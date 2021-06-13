/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-22
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** test_data_styles.cpp is part of Spectral Viewer Tests
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
#include "viewer_data_styles.h"
#include "viewer_data_factory.h"

#include "QtGui/private/qcssparser_p.h"

// Test viewer_data_style
TEST_CASE("Data Style", "[StyleBuilder]"){

    SECTION("QSS Style parsing"){
        // Starting StyleBuilder
        StyleBuilder style;

        // Starting the QCss parser input and output
        QCss::Parser parser;
        QCss::StyleSheet parser_stylesheet;

        SECTION("DEFAULT"){
            parser.init(style.getStyleSheet());
            REQUIRE(parser.parse(&parser_stylesheet));
        }

        // Check whether styles.ini can be parsed
        DataFactory factory;
        REQUIRE_FALSE(factory.isWarning());

        // Get all style id's
        std::vector<QString> style_ids;
        style_ids = style.getStyleID(factory);

        // Check whether all styles.ini can be parsed
        for(const QString& style_id : style_ids){
            SECTION(style_id.toStdString()){
                // Build stylesheet and load parser
                style.loadStyle(factory, style_id);
                parser.init(style.getStyleSheet());

                // Parse and check output 
                CHECK(parser.parse(&parser_stylesheet));
            }
        };
    }
}
