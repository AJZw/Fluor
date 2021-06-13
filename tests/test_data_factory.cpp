/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-22
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** test_data_factory.cpp is part of Spectral Viewer Tests
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

#include <QApplication>
#include <QDir>
#include <QString>

namespace Catch {
    template<> struct StringMaker<QString>{
        static std::string convert(QString const& value){
            return value.toStdString();
        }
    };
    template<> struct StringMaker<QVariant>{
        static std::string convert(QVariant const& value){
            return value.toString().toStdString();
        }
    };
}

// Test viewer_data_factory
TEST_CASE("Data Factory", "[DataFactory]"){
    
    SECTION("Path Merging"){
        // Build DataFactory with invalid entrees
        DataFactory factory("placeholder/settings.ini", "placeholder/styles.ini", "placeholder/cytometers.ini", "placeholder/fluorophores.ini");

        // Check path merging
        QString path = QApplication::applicationDirPath();
        CHECK(factory.getPathSettings() == QDir(path).filePath("placeholder/settings.ini"));
        CHECK(factory.getPathDefaults() == QDir(path).filePath("placeholder/settings.ini"));
        CHECK(factory.getPathStyles() == QDir(path).filePath("placeholder/styles.ini"));
        CHECK(factory.getPathCytometers() == QDir(path).filePath("placeholder/cytometers.ini"));
        CHECK(factory.getPathFluorophores() == QDir(path).filePath("placeholder/fluorophores.ini"));
    }

    SECTION("Factory Path Validity 1"){
        DataFactory factory("placeholder/settings.ini", "placeholder/styles.ini", "placeholder/cytometers.ini", "placeholder/fluorophores.ini");
        CHECK_FALSE(factory.isValid());
        CHECK(factory.isWarning());
    }

    SECTION("Factory Path Validity 2"){
        DataFactory factory("data/settings.ini", "placeholder/styles.ini", "placeholder/cytometers.ini", "placeholder/fluorophores.ini");
        CHECK(factory.isValid());
        CHECK(factory.isWarning());
    }

    SECTION("Factory Path Validity 3"){
        DataFactory factory("placeholder/settings.ini", "data/styles.ini", "placeholder/cytometers.ini", "placeholder/fluorophores.ini");
        CHECK_FALSE(factory.isValid());
        CHECK(factory.isWarning());
    }

    SECTION("Factory Path Validity 4"){
        DataFactory factory("placeholder/settings.ini", "placeholder/styles.ini", "data/cytometers.ini", "placeholder/fluorophores.ini");
        CHECK_FALSE(factory.isValid());
        CHECK(factory.isWarning());
    }

    SECTION("Factory Path Validity 5"){
        DataFactory factory("placeholder/settings.ini", "placeholder/styles.ini", "placeholder/cytometers.ini", "data/fluorophores.ini");
        CHECK_FALSE(factory.isValid());
        CHECK(factory.isWarning());
    }

    SECTION("Factory Path Validity 6"){
        DataFactory factory("data/settings.ini", "data/styles.ini", "data/cytometers.ini", "data/fluorophores.ini");
        CHECK(factory.isValid());
        CHECK_FALSE(factory.isWarning());
    }
}

// DataFactory::get() is not tested -> would need a death test
// DataFactory::execMessages() -> not sure how to test

// DataError & DataWarning dont warrant a unit test
