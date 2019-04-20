/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-11
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_cytometers.h"

#include <QDebug>

/*
Constructor: Construct the fluorophore data types
*/
DataCytometers::DataCytometers(){}

/*
Loads the cytometers QSettings
Note: do not call this if DataFactory 'cytometers' is invalid, that causes the factory to qFatal()
    :param data: the DataFactory to request the source data from
*/
void DataCytometers::load(DataFactory& data){
    // Retrieve QSetting
    std::unique_ptr<QSettings> cytometers;
    cytometers = data.get("cytometers");
    
    // Sets the loaded flag
    data_loaded = true;
}

/*
Unallocates memory of all loaded data
*/
void DataCytometers::unload(){
    data_loaded = false;
}

/*
Returns whether DataCytometers is valid (has loaded cytometer data)
    :returns: validity
*/
bool DataCytometers::isValid() const {
    if(this->data_loaded){
        return true;
    }else{
        return false;
    }
}



/*
class Data():
    """
    
    """
    def __init__(self):
        self.cytometers = None
        self.fluorophores = None

        # Loading .ini files
        self.loadCytometers()

    # Unloaders
    def unloadCytometers(self):
        """ Unloads the cytometers QSettings """
        self.cytometers = None

    # Reloaders
    def changeCytometer(self, cytometer):
        """
        Deselects the previously loaded (if any) cytometer and beginGroup of the new one
            :param cytometer[str/None]: the group name of the cytometer to be loaded, resets group is None is given
        """
        if self.cytometers is not None:
            if self.cytometers.group() != "":
                self.cytometers.endGroup()
            if cytometer is not None:
                self.cytometers.beginGroup(cytometer)

    # Assessors
    def getLasers(self):
        """
        Returns list of laser names and wavelengths in settings
            :return: list of lasers with each entree being a tuple of laser name and laser wavelength or False if no data is loaded
        """
        laser_list = []

        if self.cytometers is None:
            return False
        if self.cytometers.group() == "":
            return laser_list

        for laser in range(0, self.cytometers.value("laser_count", 0, type=int)):
            laser_list.append((self.cytometers.value("laser_{}_name".format(laser), "unknown"),
                               self.cytometers.value("laser_{}_wavelength".format(laser), type=int)))

        laser_list = sorted(laser_list, key=lambda wavelength: wavelength[1])

        return laser_list

    def getDetectors(self, wavelength):
        """
        Returns a dict of all detectors for the wavelength, if applicable, otherwise returns False
            :param wavelength[int]: laser wavelength
            :return: list[tuple(detector_wavelength[int], detector_FWHM[int]), ...] or False
        """
        if self.cytometers is None:
            return False
        if self.cytometers.group() == "":
            return False

        laser = 0
        valid_wavelength = False
        for laser in range(0, self.cytometers.value("laser_count", 0, type=int)):
            if self.cytometers.value("laser_{}_wavelength".format(laser), 0, type=int) == wavelength:
                valid_wavelength = True
                break

        if valid_wavelength is False:
            return False

        detectors = []
        for detector in range(0, self.cytometers.value("detector_{}_count".format(laser), 0, type=int)):
            detector_wavelength = self.cytometers.value("detector_{}_{}_wavelength".format(laser, detector), 0, type=int)

            detector_fwhm = self.cytometers.value("detector_{}_{}_fwhm".format(laser, detector), 0)
            if detector_fwhm.isdigit():
                detector_fwhm = int(detector_fwhm)
            detectors.append((detector_wavelength, detector_fwhm))

        return detectors

*/
