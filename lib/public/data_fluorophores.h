/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** data_fluorophores.h is part of Fluor
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
** The fluorophore data classes
**
** :class: DataFluorophores
** Object that loads fluorophore data from a QSettings. Builds maps/sets,
** and can return spectrum (DataSpectrum) data upon request.
** 
***************************************************************************/

#ifndef DATA_FLUOROPHORES_H
#define DATA_FLUOROPHORES_H

#include "data_factory.h"
#include "data_global.h"
#include "data_spectrum.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <QString>
#include <QStringList>



class DATALIB_EXPORT DataFluorophores {
    public:
        DataFluorophores();
        void load(DataFactory& data);
        void unload();
        bool isValid() const;

        std::unique_ptr<DataSpectrum> getSpectrum(const DataFactory& data, const QString& id) const;
        std::unique_ptr<CacheSpectrum> getCacheSpectrum(const DataFactory& data, unsigned int index, const QString& id, const QString& name) const;
        const std::vector<QString>& getFluorName() const;
        const std::unordered_map<QString, QString>& getFluorID() const;

    private:
        bool data_loaded;
        std::vector<QString> fluor_name;                    // ordered vector of fluorophore names (for input list)
        std::unordered_map<QString, QString> fluor_id;      // unordered map, each fluorophore's name corresponding fluorophore ID (for spectrum lookup)
    
        static std::vector<double> toStdVector(const QStringList& stringlist);
        static void qDebugMap(const std::unordered_map<QString, QString>& map);
        static void qDebugMap(const std::unordered_map<QString, QStringList>& map);
        static void qDebugMap(const std::unordered_map<QString, std::vector<int>>& map);
        static void qDebugSet(const std::unordered_set<QString>& map);
};

#endif // DATA_FLUOROPHORES_H
