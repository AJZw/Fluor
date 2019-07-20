/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-05-20
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** cache.h is part of Fluor
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
** 
**
***************************************************************************/

#ifndef CACHE_H
#define CACHE_H

#include "data_spectrum.h"
#include "data_fluorophores.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include <set>

namespace Cache {

// Temporarily, should be moved upon implementation of settings / menu
enum class SortOption {
    Additive, 
    AdditiveReversed, 
    Alphabetical, 
    AlphabeticalReversed, 
    Excitation, 
    ExcitationReversed, 
    Emission,
    EmissionReversed
};

struct CacheID {
    CacheID(const QString id, const QString name) : id(id), name(name), data(nullptr) {};
    CacheID(const CacheID&) = default;
    CacheID& operator=(const CacheID&) = default;
    CacheID(CacheID&&) = default;
    CacheID& operator=(CacheID&&) = default;
    ~CacheID() = default;

    friend QDebug operator<<(QDebug stream, const CacheID& object){return stream << "{" << object.id << ":" << object.name << ":" << object.data << "}";};
    bool operator<(const CacheID& other) const {return this->id < other.id;}

    QString id;
    mutable QString name;
    mutable Data::CacheSpectrum* data;
};

class Cache : public QObject {
    Q_OBJECT

    public:
        explicit Cache(Data::Factory& factory, Data::Fluorophores& source);
        Cache(const Cache &obj) = delete;
        Cache& operator=(const Cache &obj) = delete;
        Cache(Cache&&) = delete;
        Cache& operator=(Cache&&) = delete;
        ~Cache() = default;

        void printState() const;

    private:
        unsigned int counter;

        const Data::Factory& source_factory;
        const Data::Fluorophores& source_data;

        std::set<CacheID> items;
        std::unordered_map<QString, Data::CacheSpectrum> data;

        unsigned int getCounter(unsigned int size);
        Data::CacheSpectrum* getData(const QString& id, const QString& name, const unsigned int counter);

        void sync();
        static void sortVector(std::vector<CacheID>& input, SortOption option);

    public slots:
        void cacheAdd(std::set<Data::FluorophoreID>& fluorophores);
        void cacheRemove(std::set<Data::FluorophoreID>& fluorophores);

    signals:
        void syncFluor(const std::vector<CacheID>& input);

};

} // Cache namespace

#endif // CACHE_H
