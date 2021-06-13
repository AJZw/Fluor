/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_data_fluorophores.h is part of Spectral Viewer
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

/**** DOC ******************************************************************
** The stylesheet builder
**
** :class: DataColor
** Object that contains the red, green, blue values of a color
** 
** :class: DataSpectrum
** Object that contains the spectrum data of a fluorophore
** 
** :class: DataMeta
** Object that contains fluorophore meta data
** 
** :class: DataFluorophores
** Object that loads fluorophore data from a QSettings. Builds maps/sets,
** and can return spectrum (DataSpectrum) data.
** 
***************************************************************************/

#ifndef VIEWER_DATA_FLUOROPHORES_H
#define VIEWER_DATA_FLUOROPHORES_H

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <QString>
#include <QStringList>

#include "viewer_data_factory.h"

class DataColor {
    public:
        DataColor();
        DataColor(int red, int green, int blue);
        int red;
        int green;
        int blue;
};

class DataSpectrum {
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

class DataMeta {
    public:
        DataMeta();
        DataMeta(int excitation_max, int emission_max);
        int excitation_max;
        int emission_max;
};

class DataFluorophores {
    public:
        DataFluorophores();
        void load(DataFactory& data);
        void unload();
        bool isValid() const;

        std::unique_ptr<DataSpectrum> getSpectrum(const DataFactory& data, const QString& id) const;
        const std::vector<QString>& getFluorName() const;
        const std::unordered_map<QString, QString>& getFluorID() const;
        const std::unordered_map<QString, QStringList>& getFluorMultiname() const;
        const std::unordered_map<QString, DataMeta>& getFluorMeta() const;

    private:
        bool data_loaded;
        std::vector<QString> fluor_name;                    // ordered vector of fluorophore names (for input list)
        std::unordered_map<QString, QString> fluor_id;      // unordered map, each fluorophore's name corresponding fluorophore ID (for plotting)
        std::unordered_map<QString, QStringList> fluor_multiname; // unordered map, the fluorophore IDs with have alternative names (for disabling menu options)
        std::unordered_map<QString, DataMeta> fluor_meta;   // unordered map, each fluorophore ID containing meta data (for sorting)
    
        static std::vector<double> toStdVector(const QStringList& stringlist);
        static void qDebugMap(const std::unordered_map<QString, QString>& map);
        static void qDebugMap(const std::unordered_map<QString, QStringList>& map);
        static void qDebugMap(const std::unordered_map<QString, std::vector<int>>& map);
        static void qDebugSet(const std::unordered_set<QString>& map);
};

#endif // VIEWER_DATA_FLUOROPHORES_H
