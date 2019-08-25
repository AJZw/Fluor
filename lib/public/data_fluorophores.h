/**** General **************************************************************
** Version:    v0.9.4
** Date:       2019-07-21
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

#include <QDebug>
#include <QString>
#include <QStringList>

namespace Data {

struct DATALIB_EXPORT FluorophoreID {
    FluorophoreID(QString id, QString name, unsigned int order) : id(id), name(name), order(order) {};
    FluorophoreID(const FluorophoreID&) = default;
    FluorophoreID& operator=(const FluorophoreID&) = default;
    FluorophoreID(FluorophoreID&&) = default;
    FluorophoreID& operator=(FluorophoreID&&) = default;
    ~FluorophoreID() = default;

    friend QDebug operator<<(QDebug stream, const FluorophoreID& object){return stream << "{" << object.id << ":" << object.name << ":" << object.order << "}";};
    bool operator<(const FluorophoreID& other) const {return this->id < other.id;}

    const QString id;
    mutable QString name;
    mutable unsigned int order;
};

class DATALIB_EXPORT Fluorophores {
    public:
        Fluorophores();
        Fluorophores(const Fluorophores&) = default;
        Fluorophores& operator=(const Fluorophores&) = default;
        Fluorophores(Fluorophores&&) = default;
        Fluorophores& operator=(Fluorophores&&) = default;
        ~Fluorophores() = default;

        void load(Data::Factory& data);
        void unload();
        bool isValid() const;

        const std::vector<QString>& getFluorName() const;
        const std::unordered_map<QString, QString>& getFluorID() const;
        const std::unordered_map<QString, QStringList>& getFluorNames() const;

        Data::Spectrum getSpectrum(const Data::Factory& data, const QString& id) const;
        Data::CacheSpectrum getCacheSpectrum(const Data::Factory& data, const QString& id, unsigned int index) const;

    private:
        bool data_loaded;
        std::vector<QString> fluor_name;                        // ordered vector of fluorophore names (for input list)
        std::unordered_map<QString, QString> fluor_id;          // unordered map, each fluorophore's name corresponding fluorophore ID (for ID/spectrum lookup)
        std::unordered_map<QString, QStringList> fluor_names;   // unordered map, each fluorophore's name corresponding to all name variants (for lineedit item en/disabling) 
    
        static Data::Polygon toPolygon(const QStringList& list_x, const QStringList& list_y);

        static void qDebugMap(const std::unordered_map<QString, QString>& map);
        static void qDebugMap(const std::unordered_map<QString, QStringList>& map);
        static void qDebugMap(const std::unordered_map<QString, std::vector<double>>& map);
        static void qDebugSet(const std::unordered_set<QString>& map);
};

}   // Data namespace

#endif // DATA_FLUOROPHORES_H
