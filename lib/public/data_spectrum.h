/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** data_spectrum.h is part of Fluor
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
** The spectrum objects
**
** :class: DataColor
** Struct that contains the red, green, blue values of a color
** 
** :class: DataMeta
** Struct for Spectrum meta data
**
** :class: DataSpectrum
** Object that contains the spectrum data of a fluorophore
**
** :class: CacheSpectrum
** Object container for spectrum and meta data for use in cache
** 
***************************************************************************/

#ifndef DATA_SPECTRUM_H
#define DATA_SPECTRUM_H

#include "data_global.h"

#include <QString>
#include <QStringList>
#include <vector>


class DATALIB_EXPORT DataColor {
    public:
        DataColor();
        DataColor(int red, int green, int blue);
        int red;
        int green;
        int blue;
};

class DATALIB_EXPORT DataMeta {
    public:
        DataMeta();
        DataMeta(int excitation_max, int emission_max);
        int excitation_max;
        int emission_max;
};

class DATALIB_EXPORT DataSpectrum {
    public:
        DataSpectrum(QString fluor_id);
        DataSpectrum(
            QString fluor_id,
            std::vector<double> excitation_wavelength, 
            std::vector<double> excitation_intensity, 
            std::vector<double> emission_wavelength,
            std::vector<double> emission_intensity
        );

        QString id() const;
        bool isValid() const;

        void setExcitation(const std::vector<double> wavelength, const std::vector<double> intensity);
        void setEmission(const std::vector<double> wavelength, const std::vector<double> intensity);

        // All further functions assume a valid object
        // All the getters return a copy, maybe make the returns shared/unique_ptr?
        // Copy return does enable placing on the stack
        std::vector<double> getExcitationWavelength() const;
        std::vector<double> getExcitationIntensity() const;
        std::vector<double> getEmissionWavelength() const;
        std::vector<double> getEmissionIntensity() const;
        std::vector<double> getEmissionIntensity(const double intensity, const double cutoff=0.0) const ;

        double excitationAt(const int wavelength) const;
        double emissionAt(const int wavelength) const;
        double excitationMax() const;
        double emissionMax() const;

        static DataColor color(const double wavelength); // instead of DataColor return QColor?

    private:
        const QString fluor_id;

        std::vector<double> excitation_wavelength;
        std::vector<double> excitation_intensity;
        std::vector<double> emission_wavelength;
        std::vector<double> emission_intensity;

        // wavelength lookup can be further optimized by assuming that the stepsize is 1 -> you can calculate the exact index (wavelength - front())
        static double intensityAt(const std::vector<double>& wavelength, const std::vector<double>& intensity, const int& at_wavelength);
};

// this class could inherit QObject to allow for signal/slot mechanics
class DATALIB_EXPORT CacheSpectrum {
    public:
        CacheSpectrum(unsigned int index, DataSpectrum spectrum);
        CacheSpectrum(unsigned int index, QString fluor_name, DataSpectrum spectrum);
        CacheSpectrum(unsigned int index, QString fluor_name, QStringList fluor_names, DataSpectrum spectrum, DataMeta meta);

        unsigned int index() const;
        QString id() const;
        QString name() const;
        QStringList names() const;

        double excitationMax() const;
        double emissionMax() const;

        bool visibleExcitation() const;
        bool visibleEmission() const;
        void setVisibleExcitation(bool visibility);
        void setVisibleEmission(bool visibility);

        double intensityCutoff() const;
        void setIntensityCutoff(const double cutoff);

        std::vector<double> getExcitationWavelength() const;
        std::vector<double> getExcitationIntensity() const;
        std::vector<double> getEmissionWavelength() const;
        std::vector<double> getEmissionIntensity() const;
        std::vector<double> getEmissionIntensity(const double intensity) const;

    private:
        const unsigned int build_index;

        const DataSpectrum fluor_spectrum;

        const QString fluor_name;
        const QStringList fluor_all_names;
        const DataMeta fluor_meta;

        bool fluor_visible_excitation;
        bool fluor_visible_emission;
        double fluor_intensity_cutoff;
};

#endif // DATA_SPECTRUM_H
