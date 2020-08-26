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
** The storage and handling of the global Spectra objects, named the cache 
**
** :class: Cache::CacheID
** Struct of the Spectrum ID, name, and data pointer. This is the way spectra
** are stored within the cache items
**
** :class: Cache::CacheState
** Storage class of State::GUIGlobal properties for proper item instantiation
**
** :class: Cache::Cache
** Caching, handling, and synchronisation of Spectra for showing in the GUI
** Keeps a std::set 'items' for all currently active spectra, stored as CacheID
** Keeps a std::unordered_map 'data' for all loaded spectra data
**
***************************************************************************/

#ifndef CACHE_H
#define CACHE_H

#include "global.h"
#include "data_spectrum.h"
#include "data_fluorophores.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include <set>

namespace Cache {

struct ID {
    ID(const QString id, const QString name) : id(id), name(name), data(nullptr) {};
    ID(const ID&) = default;
    ID& operator=(const ID&) = default;
    ID(ID&&) = default;
    ID& operator=(ID&&) = default;
    ~ID() = default;

    friend QDebug operator<<(QDebug stream, const ID& object){return stream << "{" << object.id << ":" << object.name << ":" << object.data << "}";};
    bool operator<(const ID& other) const {return this->id < other.id;}

    QString id;
    mutable QString name;
    mutable Data::CacheSpectrum* data;
};

struct Settings {
    bool visible_excitation = false;
    bool visible_emission = true;
    State::SortMode sort_mode = State::SortMode::Additive;
};

class Cache {
    public:
        explicit Cache(Data::Factory& factory, Data::FluorophoreReader& source);
        Cache(const Cache &obj) = delete;
        Cache& operator=(const Cache &obj) = delete;
        Cache(Cache&&) = delete;
        Cache& operator=(Cache&&) = delete;
        ~Cache() noexcept = default;

        void printState() const;

    private:
        // For cache functioning
        unsigned int counter = 0;
        unsigned int max_cache_size = 25;

        const Data::Factory& source_factory;
        const Data::FluorophoreReader& source_data;

        std::set<ID> items;
        std::unordered_map<QString, Data::CacheSpectrum> data;

        // For proper Cache item initiating, requires to know some general properties:
        Settings cache_settings;

    private:
        unsigned int getCounter(unsigned int size);
        Data::CacheSpectrum* getData(const QString& id, const unsigned int counter);
        void rebuildCounter();
        void rebuildCache();

        // void sync();
        // void update();
        static void sortVector(std::vector<ID>& input, State::SortMode mode);

    public:
        void add(std::vector<Data::FluorophoreID>& fluorophores);
        void remove(std::vector<Data::FluorophoreID>& fluorophores);

        Settings settings() const;
        void setSettings(Settings settings);
        void setSettingsExcitation(bool visible);
        void setSettingsEmission(bool visible);
        void setSettingsSorting(State::SortMode mode);

        const std::vector<ID> state() const;
};

} // Cache namespace

#endif // CACHE_H
