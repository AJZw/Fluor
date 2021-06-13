/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-22
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** test_file_fluorophores.cpp is part of Spectral Viewer Tests
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

TEST_CASE("File Fluorophores", "[Fluorophores.ini]"){
    // Make sure the ini data files exists
    DataFactory factory;
    REQUIRE_FALSE(factory.isWarning());

    // Work with 'RAW' QSettings, might want to move these functions into a class eventually
    std::unique_ptr<QSettings> fluorophores;
    fluorophores = factory.get("fluorophores");

    // Test whether it contains atleast 1 entree
    QStringList fluorophore_ids = fluorophores->childGroups();
    REQUIRE(fluorophore_ids.size() >= 1);

    SECTION("DEFAULT"){
        REQUIRE(fluorophore_ids.contains("DEFAULT"));
        fluorophores->beginGroup("DEFAULT");
        QStringList keys = fluorophores->childKeys();

        SECTION("key: enable"){
            REQUIRE(keys.contains("enable"));
            QString enable = fluorophores->value("enable", "").toString();
            CHECK((enable == "true" || enable == "false"));
        }

        SECTION("key: excitation_max"){
            REQUIRE(keys.contains("excitation_max"));
            QString excitation_max = fluorophores->value("excitation_max", "").toString();
            int excitation_max_int = excitation_max.toInt();

            //toInt() on non-numeric QString defaults to zero, so first check if it is a 'valid' zero or parsing error
            if(excitation_max != "0"){
                CHECK(excitation_max_int != 0);
            }
            
            // Check bounds
            CHECK(excitation_max_int >= 0);
            CHECK(excitation_max_int <= 2000);
        }

        SECTION("key: emission_max"){
            REQUIRE(keys.contains("emission_max"));
            QString emission_max = fluorophores->value("emission_max", "").toString();
            int emission_max_int = emission_max.toInt();

            //toInt() on non-numeric QString defaults to zero, so first check if it is a 'valid' zero or parsing error
            if(emission_max != "0"){
                CHECK(emission_max_int != 0);
            }
            
            // Check bounds
            CHECK(emission_max_int >= 0);
            CHECK(emission_max_int <= 2000);
        }

        fluorophores->endGroup();
    }

    for(const QString& fluorophore_id : fluorophore_ids){
        if(fluorophore_id == "DEFAULT"){
            continue;
        }
        SECTION(fluorophore_id.toStdString()){
            fluorophores->beginGroup(fluorophore_id);
            QStringList keys = fluorophores->childKeys();

            if(keys.contains("enable")){
                SECTION("key: enable"){
                    QString enable = fluorophores->value("enable", "").toString();
                    CHECK((enable == "true" || enable == "false"));
                }
            }
            
            if(keys.contains("alternative_name")){
                SECTION("key: alternative name"){
                    QStringList alternative_name = fluorophores->value("alternative_name").toStringList();
                    CHECK_FALSE(alternative_name.isEmpty());
                }
            }

            if(keys.contains("excitation_max")){
                SECTION("key: excitation_max"){
                    QString excitation_max = fluorophores->value("excitation_max", "").toString();
                    int excitation_max_int = excitation_max.toInt();

                    //toInt() on non-numeric QString defaults to zero, so first check if it is a 'valid' zero or parsing error
                    if(excitation_max != "0"){
                        CHECK(excitation_max_int != 0);
                    }
                    
                    // Check bounds
                    CHECK(excitation_max_int >= 0);
                    CHECK(excitation_max_int <= 2000);
                }
            }

            if(keys.contains("emission_max")){
                SECTION("key: emission_max"){
                    QString emission_max = fluorophores->value("emission_max", "").toString();
                    int emission_max_int = emission_max.toInt();

                    //toInt() on non-numeric QString defaults to zero, so first check if it is a 'valid' zero or parsing error
                    if(emission_max != "0"){
                        CHECK(emission_max_int != 0);
                    }
                    
                    // Check bounds
                    CHECK(emission_max_int >= 0);
                    CHECK(emission_max_int <= 2000);
                }
            }

            SECTION("key: excitation"){
                REQUIRE(keys.contains("excitation_wavelength"));
                REQUIRE(keys.contains("excitation_intensity"));

                // Loads as string to check if it contains 
                QStringList wavelength = fluorophores->value("excitation_wavelength").toStringList();
                QStringList intensity = fluorophores->value("excitation_intensity").toStringList();

                SECTION("equal size"){
                    CHECK(wavelength.size() == intensity.size());
                }

                // Transform wavelength to double while checking for 'valid' transformation
                bool only_numbers = true;
                std::vector<double> wavelength_double(wavelength.size());
                for(int i=0; i<wavelength.size(); ++i){
                    wavelength_double[i] = wavelength[i].toDouble();
                    
                    if((wavelength[i] != "0" && wavelength[i] != "0.0") && wavelength_double[i] == 0){
                        only_numbers = false;
                    }
                }

                SECTION("wavelength: only numbers"){
                    CHECK(only_numbers);
                }

                // Transform intensity to double while checking for 'valid' transformation
                only_numbers = true;
                std::vector<double> intensity_double(intensity.size());
                for(int i=0; i<intensity.size(); ++i){
                    intensity_double[i] = intensity[i].toDouble();
                    
                    if((intensity[i] != "0" && intensity[i] != "0.0") && intensity_double[i] == 0){
                        only_numbers = false;
                    }
                }

                SECTION("intensity: only numbers"){
                    CHECK(only_numbers);
                }

                // Check wavelength stepsize and order
                bool stepsize_1 = true;
                bool in_order = true;
                for(unsigned int i=1; i<wavelength_double.size(); ++i){
                    if(wavelength_double[i-1] >= wavelength_double[i]){
                        in_order = false;
                    }
                    if(wavelength_double[i-1] + 1 != wavelength_double[i]){
                        stepsize_1 = false;
                    }
                }
                
                SECTION("wavelength: order"){
                    CHECK(stepsize_1);
                }
                SECTION("wavelength: stepsize"){
                    CHECK(in_order);
                }

                bool intensity_min = false;
                bool intensity_max = false;
                // Test whether intensity is above or below min(0) and max(100)
                for(unsigned int i=0; i<intensity_double.size(); ++i){
                    if(intensity_double[i] < 0){
                        intensity_min = true;
                    }
                    if(intensity_double[i] > 100){
                        intensity_max = true;
                    }
                }

                SECTION("intensity: min < 0"){
                    CHECK_FALSE(intensity_min);
                }
                SECTION("intensity: max >100"){
                    CHECK_FALSE(intensity_max);
                }
            }

            SECTION("key: emission"){
                REQUIRE(keys.contains("emission_wavelength"));
                REQUIRE(keys.contains("emission_intensity"));

                // Loads as string to check if it contains 
                QStringList wavelength = fluorophores->value("emission_wavelength").toStringList();
                QStringList intensity = fluorophores->value("emission_intensity").toStringList();

                SECTION("equal size"){
                    CHECK(wavelength.size() == intensity.size());
                }

                // Transform wavelength to double while checking for 'valid' transformation
                bool only_numbers = true;
                std::vector<double> wavelength_double(wavelength.size());
                for(int i=0; i<wavelength.size(); ++i){
                    wavelength_double[i] = wavelength[i].toDouble();
                    
                    if((wavelength[i] != "0" && wavelength[i] != "0.0") && wavelength_double[i] == 0){
                        only_numbers = false;
                    }
                }

                SECTION("wavelength: only numbers"){
                    CHECK(only_numbers);
                }

                // Transform intensity to double while checking for 'valid' transformation
                only_numbers = true;
                std::vector<double> intensity_double(intensity.size());
                for(int i=0; i<intensity.size(); ++i){
                    intensity_double[i] = intensity[i].toDouble();
                    
                    if((intensity[i] != "0" && intensity[i] != "0.0") && intensity_double[i] == 0){
                        only_numbers = false;
                    }
                }

                SECTION("intensity: only numbers"){
                    CHECK(only_numbers);
                }

                // Check wavelength stepsize and order
                bool stepsize_1 = true;
                bool in_order = true;
                for(unsigned int i=1; i<wavelength_double.size(); ++i){
                    if(wavelength_double[i-1] >= wavelength_double[i]){
                        in_order = false;
                    }
                    if(wavelength_double[i-1] + 1 != wavelength_double[i]){
                        stepsize_1 = false;
                    }
                }
                
                SECTION("wavelength: order"){
                    CHECK(stepsize_1);
                }
                SECTION("wavelength: stepsize"){
                    CHECK(in_order);
                }

                bool intensity_min = false;
                bool intensity_max = false;
                // Test whether intensity is above or below min(0) and max(100)
                for(unsigned int i=0; i<intensity_double.size(); ++i){
                    if(intensity_double[i] < 0){
                        intensity_min = true;
                    }
                    if(intensity_double[i] > 100){
                        intensity_max = true;
                    }
                }

                SECTION("intensity: min >= 0"){
                    CHECK_FALSE(intensity_min);
                }
                SECTION("intensity: max  <= 100"){
                    CHECK_FALSE(intensity_max);
                }
            }

            fluorophores->endGroup();
        }
    }
}


