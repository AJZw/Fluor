/**** General **************************************************************
** Version:    v0.9.11
** Date:       2020-10-27
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License ******************************************************** 
** data_instruments.h is part of Fluor
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
** :class: Data::Laser
** Data representation of a Laser in a laser line
**
** :class: Data::Filter
** Data representation of a Detector's Filter in a laser line
**
** :class: Data::LaserLine
** Data representation of a line of lasers with filters that are part of the same light path
**
** :class: Data::LaserID
** Data representation of a Laser (belonging to a laserline)
** Class is purily used for communication with the GUI
**
** :class: Data::InstrumentID
** The ID and name of an Instrument in instruments.ini
**
** :class: Data::Instrument
** A class representation of a instrument
**
** :class: Data::InstrumentReader
** A Reader object that loads the instruments.ini and can return a instruments laser/filter properties
** 
***************************************************************************/

#ifndef DATA_INSTRUMENTS_H
#define DATA_INSTRUMENTS_H

#include "data_global.h"
#include "data_factory.h"
#include <QDebug>
#include <QString>
#include <vector>
#include <unordered_map>

namespace Data {

class DATALIB_EXPORT Laser {
    public:
        Laser(double wavelength, QString name=QString());
        Laser(const Laser&) = default;
        Laser& operator=(const Laser&) = default;
        Laser(Laser&&) = default;
        Laser& operator=(Laser&&) = default;
        ~Laser() = default;

    private:
        double laser_wavelength;
        QString laser_name;

    public:
        friend QDebug operator<<(QDebug stream, const Laser& object){return stream << "{" << object.laser_wavelength << "nm}";};
        bool operator<(const Laser& other) const;
        bool operator>(const Laser& other) const;
        bool operator<=(const Laser& other) const;
        bool operator>=(const Laser& other) const;
        bool operator==(const Laser& other) const;
        bool operator!=(const Laser& other) const;

        double wavelength() const;
        const QString& name() const;
};

class DATALIB_EXPORT Filter {
    public:
        enum Type {BandPass, LongPass, ShortPass};

        Filter(Filter::Type type, double wavelength, double fwhm=0.0, QString name=QString());
        Filter(const Filter&) = default;
        Filter& operator=(const Filter&) = default;
        Filter(Filter&&) = default;
        Filter& operator=(Filter&&) = default;
        ~Filter() = default;

    private:
        Type filter_type;
        double filter_wavelength;
        double filter_fwhm;
        QString filter_name;

    public:
        friend QDebug operator<<(QDebug stream, const Filter& object){
            if(object.filter_type == Filter::BandPass){
                return stream << "{BP:" << object.filter_wavelength << ":" << object.filter_fwhm << "}";
            }else if(object.filter_type == Filter::LongPass){
                return stream << "{LP:" << object.filter_wavelength << "}";
            }else if(object.filter_type == Filter::ShortPass){
                return stream << "{SP:" << object.filter_wavelength << "}";
            }else{
                qWarning() << "QDebug Data::Detector::operator<<: Unknown Detector::Type";
                return stream;
            }
        };
        Filter::Type type() const;
        double wavelength() const;
        double fwhm() const;
        const QString& name() const;
        double wavelengthMin() const;
        double wavelengthMax() const;
};

class DATALIB_EXPORT LaserLine {
    public:
        LaserLine();
        LaserLine(std::size_t reserve_lasers, std::size_t reserve_filters);
        LaserLine(const LaserLine&) = default;
        LaserLine& operator=(const LaserLine&) = default;
        LaserLine(LaserLine&&) = default;
        LaserLine& operator=(LaserLine&&) = default;
        ~LaserLine() = default;

    private:
        std::vector<Laser> line_lasers;
        std::vector<Filter> line_filters;

    public:
        friend QDebug operator<<(QDebug stream, const LaserLine& object){return stream << "{laser(" << object.line_lasers.size() << "):filters(" << object.line_filters.size() << ")}";};

        std::vector<Laser>& lasers();
        const std::vector<Laser>& lasers() const;
        std::vector<Filter>& filters();
        const std::vector<Filter>& filters() const;

        bool isValid() const;
        void sort();
};

struct DATALIB_EXPORT LaserID {
    LaserID(const Data::Laser* laser, const Data::LaserLine* laserline=nullptr) : 
        laser(laser),
        laserline(laserline),
        custom_wavelength(0.0)
    {};
    LaserID(const LaserID&) = default;
    LaserID& operator=(const LaserID&) = default;
    LaserID(LaserID&&) = default;
    LaserID& operator=(LaserID&&) = default;
    ~LaserID() = default;

    friend QDebug operator<<(QDebug stream, const LaserID& object){
        if(object.custom_wavelength != 0.0){
            return stream << "{C:" << object.custom_wavelength << "nm}";
        }else{
            return stream << "{" << object.laser->wavelength() << "nm}";
        }
    };

    const Data::Laser* laser;
    const Data::LaserLine* laserline;
    double custom_wavelength;
};

struct DATALIB_EXPORT InstrumentID {
    InstrumentID(QString id, QString name) : id(std::move(id)), name(std::move(name)) {};
    InstrumentID(const InstrumentID&) = default;
    InstrumentID& operator=(const InstrumentID&) = default;
    InstrumentID(InstrumentID&&) = default;
    InstrumentID& operator=(InstrumentID&&) = default;
    ~InstrumentID() = default;

    friend QDebug operator<<(QDebug stream, const InstrumentID& object){return stream << "{" << object.id << ":" << object.name << "}";};
    bool operator<(const InstrumentID& other) const {return this->id < other.id;};
    bool operator>(const InstrumentID& other) const {return this->id > other.id;};
    bool operator<=(const InstrumentID& other) const {return this->id <= other.id;};
    bool operator>=(const InstrumentID& other) const {return this->id >= other.id;};
    bool operator==(const InstrumentID& other) const {return this->id == other.id;};
    bool operator!=(const InstrumentID& other) const {return this->id != other.id;};

    QString id;
    mutable QString name;
};

class DATALIB_EXPORT Instrument {
    public:
        Instrument();
        Instrument(QString id, QString name, std::size_t laserline_count);
        Instrument(const Instrument&) = default;
        Instrument& operator=(const Instrument&) = default;
        Instrument(Instrument&&) = default;
        Instrument& operator=(Instrument&&) = default;
        ~Instrument() = default;

    private:
        QString instrument_id;
        QString instrument_name;

        std::vector<LaserLine> instrument_optics;

    public:
        friend QDebug operator<<(QDebug stream, const Instrument& object){return stream << "{" << object.instrument_id << ":laserlines(" << object.instrument_optics.size() << ")}";};
        const QString& id() const;
        const QString& name() const;
        
        bool isValid() const;
        bool isEmpty() const;
        
        void sort();
        std::pair<const Data::LaserLine*, const Data::Laser*> findLaser(double wavelength) const;

        std::vector<LaserLine>& setOptics();
        const std::vector<LaserLine>& optics() const;
};

class DATALIB_EXPORT InstrumentReader {
    public:
        InstrumentReader();
        InstrumentReader(const InstrumentReader&) = default;
        InstrumentReader& operator=(const InstrumentReader&) = default;
        InstrumentReader(InstrumentReader&&) = default;
        InstrumentReader& operator=(InstrumentReader&&) = default;
        ~InstrumentReader() = default;

    private:
        QJsonDocument instrument_data;
        std::vector<InstrumentID> instrument_ids;

    public:
        void load(const Data::Factory& factory);
        void unload();
        bool isValid() const;

        Instrument getInstrument(const QString& id) const;
};

} // Data namespace

#endif //DATA_INSTRUMENTS_H