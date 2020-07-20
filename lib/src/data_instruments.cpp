/**** General **************************************************************
** Version:    v0.9.6
** Date:       2020-01-04
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_instruments.h"

#include <QDebug>

#include <memory>


namespace Data {

/* ############################################################################################################## */

/*
Constructor: Construct a Laser (storage) object
    :param wavelength: the central wavelength (in nm) of the laser
    :param name: (optional) the name 
*/
Laser::Laser(double wavelength, QString name) : 
    laser_wavelength(wavelength), 
    laser_name(std::move(name))
{}

/*
Comparison operators:
*/
bool Laser::operator<(const Laser& other) const {
    return this->laser_wavelength < other.laser_wavelength;
}
bool Laser::operator>(const Laser& other) const {
    return this->laser_wavelength > other.laser_wavelength;
}
bool Laser::operator<=(const Laser& other) const {
    return this->laser_wavelength <= other.laser_wavelength;
}
bool Laser::operator>=(const Laser& other) const {
    return this->laser_wavelength >= other.laser_wavelength;
}
bool Laser::operator==(const Laser& other) const {
    return this->laser_wavelength == other.laser_wavelength;
}
bool Laser::operator!=(const Laser& other) const {
    return this->laser_wavelength != other.laser_wavelength;
}

/*
Getter: gets the central wavelength of the laser object
*/
double Laser::wavelength() const {
    return this->laser_wavelength;
}

/*
Getter: gets the name specifyer of the laser
*/
const QString& Laser::name() const {
    return this->laser_name;
}

/* ############################################################################################################## */

/*
Constructor: constructs a filter object of a specified type. 
    :param type: filter type
    :param wavelength: the central (BandPass) or Edge (LongPass/ShortPass) wavelengths (in nm)
    :param fwhm: full width at half maximum (in nm) only relevant for BandPass filters
    :param name: the name specifyer of the filter
*/
Filter::Filter(Filter::Type type, double wavelength, double fwhm, QString name) : 
    filter_type(type),
    filter_wavelength(wavelength),
    filter_fwhm(fwhm),
    filter_name(name)
{}

/*
Getter: returns the type of the filter
*/
Filter::Type Filter::type() const {
    return this->filter_type;
}

/*
Getter: returns the name of the filter
*/
const QString& Filter::name() const {
    return this->filter_name;
}

/*
Getter: returns the central/edge wavelength of the filter
*/
double Filter::wavelength() const {
    return this->filter_wavelength;
}

/*
Getter: returns the full width half maximum (if relevant, otherwise 0.0)
*/
double Filter::fwhm() const {
    return this->filter_fwhm;
}

/*
Return the wavelength minimum (in nm) of the filter
*/
double Filter::wavelengthMin() const {
    if(this->filter_type == Filter::BandPass){
        return this->filter_wavelength - (0.5 * this->filter_fwhm);
    }else if(this->filter_type == Filter::LongPass){
        return this->filter_wavelength;
    }else if(this->filter_type == Filter::ShortPass){
        return 0.0;
    }else{
        qWarning() << "QDebug Data::Filter::operator<<: Unknown Filter::Type";
        return 0.0;
    }
}

/*
Return the wavelength maximum (in nm) of the filter
*/
double Filter::wavelengthMax() const {
    if(this->filter_type == Filter::BandPass){
        return this->filter_wavelength + (0.5 * this->filter_fwhm);
    }else if(this->filter_type == Filter::LongPass){
        return std::numeric_limits<double>::max();
    }else if(this->filter_type == Filter::ShortPass){
        return this->filter_wavelength;
    }else{
        qWarning() << "QDebug Data::Filter::operator<<: Unknown Filter::Type";
        return std::numeric_limits<double>::max();
    }
}

/* ############################################################################################################## */

/*
Default Constructor: does not reserve any space for the internal storage vectors
*/
LaserLine::LaserLine() :
    line_lasers(),
    line_filters()
{
    this->line_lasers.reserve(0);
    this->line_filters.reserve(0);
}

/*
Constructor: for a laserline object, reserves specific sizes for the internal storage vectors
    :param reserve_lasers: reserve size of the std::vector<Laser>
    :param reserve_filters: reserve size of the std::vector<Filter>
*/
LaserLine::LaserLine(std::size_t reserve_lasers, std::size_t reserve_filters) : 
    line_lasers(),
    line_filters()
{
    this->line_lasers.reserve(reserve_lasers);
    this->line_filters.reserve(reserve_filters);
}

/*
Getter/Setter: returns a non-const reference to the Laser storage vector
*/
std::vector<Laser>& LaserLine::lasers(){
    return this->line_lasers;
}

/*
Getter/Setter: returns a non-const reference to the Laser storage vector
*/
const std::vector<Laser>& LaserLine::lasers() const{
    return this->line_lasers;
}

/*
Getter/Setter: returns a non-const reference to the Filter storage vector 
*/
std::vector<Filter>& LaserLine::filters(){
    return this->line_filters;
}

/*
Getter/Setter: returns a non-const reference to the Filter storage vector 
*/
const std::vector<Filter>& LaserLine::filters() const {
    return this->line_filters;
}

/*
Checks whether the LaserLine is valid (aka contains valid lasers, and filters)
*/
bool LaserLine::isValid() const {
    if(this->line_lasers.size() == 0){
        return false;
    }
    for(auto& laser : this->line_lasers){
        if(laser.wavelength() == 0){
            return false;
        }
    }
    for(auto& filter : this->line_filters){
        if(filter.wavelength() == 0){
            return false;
        }
        if(filter.type() == Filter::BandPass){
            if(filter.fwhm() == 0){
                return false;
            }
        }
    }
    return true;
}

/*
Sorts the lasers and detectors in wavelength order (low-to-high).
*/
void LaserLine::sort(){
    // Sort the laser
    std::sort(
        this->line_lasers.begin(),
        this->line_lasers.end()
    );
    // Sort the filters
    std::sort(
        this->line_filters.begin(),
        this->line_filters.end(),
        [](const Filter& a, const Filter& b){return a.wavelength() < b.wavelength();}
    );
}

/* ############################################################################################################## */

/*
Constructor: default constructs an empty instrument object
*/
Instrument::Instrument() :
    instrument_id(QString()),
    instrument_name(QString()),
    instrument_optics()
{}

/*
Constructor: Container of a optical instrument
*/
Instrument::Instrument(QString id, QString name, std::size_t laserline_count) :
    instrument_id(std::move(id)),
    instrument_name(std::move(name)),
    instrument_optics()
{
    this->instrument_optics.reserve(laserline_count);
}

/*
Getter: returns the id of the instrument
*/
const QString& Instrument::id() const {
    return this->instrument_id;
}

/*
Getter: returns the name of the instrument
*/
const QString& Instrument::name() const {
    return this->instrument_name;
}

/*
Setter: returns a reference to modify the internal optics vector
*/
std::vector<LaserLine>& Instrument::setOptics(){
    return this->instrument_optics;
}

/*
Getter: returns the optics of the instrument
*/
const std::vector<LaserLine>& Instrument::optics() const {
    return this->instrument_optics;
}

/*
Returns the validity of the optics of the instrument
*/
bool Instrument::isValid() const {
    for(auto& laserline : this->instrument_optics){
        if(!laserline.isValid()){
            return false;
        }
    }
    return true;
}

/*
Returns whether the instrument is empty
*/
bool Instrument::isEmpty() const {
    if(this->instrument_optics.size() == 0){
        return true;
    }else{
        return false;
    }
}

/*
Sorts the internal LaserLines based on the wavelength of the first Laser.
Assumes there is atleast 1 laser entree / laserline (aka Instrument must be valid)
*/
void Instrument::sort(){
    // First sort the laserlines inself
    for(auto& line : this->instrument_optics){
        line.sort();
    }

    // Then sort the LaserLines
    std::sort(
        this->instrument_optics.begin(),
        this->instrument_optics.end(),
        [](LaserLine& a, LaserLine& b){return a.lasers()[0] < b.lasers()[0];}
    );
}

/*
Checks whether a laser with the specified wavelength exists and if so returns a pointer to the laserline and laser
    :param wavelength: the laser wavelength (in nm)
*/
std::pair<const Data::LaserLine*, const Data::Laser*> Instrument::findLaser(double wavelength) const {
    if(this->isEmpty()){
        return {nullptr, nullptr};
    }

    for(std::size_t i=0; i < this->instrument_optics.size(); ++i){
        auto iterator = std::find_if(
            this->instrument_optics[i].lasers().cbegin(),
            this->instrument_optics[i].lasers().cend(),
            [wavelength](const Laser& a){return a.wavelength() == wavelength;}
        );
        if(iterator != this->instrument_optics[i].lasers().cend()){
            return {&this->instrument_optics[i], std::addressof(*iterator)};
        }
    }

    return {nullptr, nullptr};
}

/* ############################################################################################################## */

/*
Constructor: Construct the Cytometers data type
*/
InstrumentReader::InstrumentReader() :
    data_loaded(false),
    data_valid(true)
{}

/*
Loads the cytometers QSettings
Note: do not call this if Data::Factory::isValid(Factory::Instruments) is invalid, that causes the factory to qFatal()
    :param data: the Data::Factory to request the source data from
*/
void InstrumentReader::load(const Data::Factory& factory){
    // Retrieve QSetting
    std::unique_ptr<QSettings> data = factory.get(Data::Factory::Instruments);

    // Reserve the necessary vector size
    std::size_t data_size = static_cast<std::size_t>(data->allKeys().size());

    this->instrument_ids.reserve(data_size);

    for(const QString &group : data->childGroups()){
        data->beginGroup(group);

        QString group_name = data->value("name", QString()).toString();

        // If instrument name was not defined, use the id as alternative name
        if(group_name.isNull()){
            group_name = group;
        }
        
        this->instrument_ids.push_back(InstrumentID(group, group_name));

        data->endGroup();
    }

    // Sort instruments_ids based on (case insensitive) alphabetical order of the name
    std::sort(
        this->instrument_ids.begin(), 
        this->instrument_ids.end(), 
        [](const InstrumentID& a, const InstrumentID& b){return a.name.toLower() < b.name.toLower();}
    );

    // Sets the loaded flag
    data_loaded = true;
}

/*
Retreives the specified instrument from the data file
    :param data: the DataFactory which builds a QSettings object containing the instrument data
*/
Instrument InstrumentReader::getInstrument(const Data::Factory& factory, const QString& id) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> data = factory.get(Data::Factory::Instruments);

    // Loads the requested group
    data->beginGroup(id);

    // Retreive data for Instrument construction
    std::size_t size = static_cast<std::size_t>(data->value("laserline_count", 0).toInt());
    QString name = data->value("name","").toString();
    Instrument machine(id, name, size);
  
    // Construct a QString to hold all keys
    // Reserve enough memory to fit all expected text within instruments.ini
    QString key = QString();
    key.reserve(35);

    // Retreive the optics layout
    std::vector<LaserLine>& optics = machine.setOptics();

    // Load each laserline
    for(std::size_t laserline=0; laserline<optics.capacity(); ++laserline){
        QString laserline_string = QString::number(laserline);

        // Acquire the lasers_count
        key.clear();
        key.append("laserline_").append(laserline_string).append("_laser_count");

        std::size_t size_lasers = static_cast<std::size_t>(data->value(key, 0).toInt());

        // Acquire the filter_count
        key.clear();
        key.append("laserline_").append(laserline_string).append("_filter_count");

        std::size_t size_filters = static_cast<std::size_t>(data->value(key, 0).toInt());

        optics.push_back(LaserLine(size_lasers, size_filters));

        // Now acquire the laser data
        for(std::size_t laser=0; laser<size_lasers; ++laser){
            QString laser_string = QString::number(laser);
            key.clear();
            key.append("laserline_").append(laserline_string).append("_laser_").append(laser_string).append("_name");
            QString name = data->value(key, "").toString();

            key.clear();
            key.append("laserline_").append(laserline_string).append("_laser_").append(laser_string).append("_wavelength");
            double wavelength = data->value(key, 0.0).toDouble();

            optics[laserline].lasers().push_back(Laser(wavelength, name));
        }
        
        // Now acquire the filter data
        for(std::size_t filter=0; filter<size_filters; ++filter){
            QString filter_string = QString::number(filter);
            key.clear();
            key.append("laserline_").append(laserline_string).append("_filter_").append(filter_string).append("_name");
            QString name = data->value(key, "").toString();

            key.clear();
            key.append("laserline_").append(laserline_string).append("_filter_").append(filter_string).append("_type");
            QString type = data->value(key, "SP").toString();

            key.clear();
            key.append("laserline_").append(laserline_string).append("_filter_").append(filter_string).append("_wavelength");
            double wavelength = data->value(key, 0.0).toDouble();

            if(type == "BP"){
                key.clear();
                key.append("laserline_").append(laserline_string).append("_filter_").append(filter_string).append("_fwhm");
                double fwhm = data->value(key, 0.0).toDouble();

                optics[laserline].filters().push_back(Filter(Filter::BandPass, wavelength, fwhm, name));
            }else if(type == "LP"){
                optics[laserline].filters().push_back(Filter(Filter::LongPass, wavelength, 0.0, name));
            }else if(type == "SP"){
                optics[laserline].filters().push_back(Filter(Filter::ShortPass, wavelength, 0.0, name));
            }
        }
    }
 
    data->endGroup();

    // Check if the instrument is valid, if not, generate warning
    if(machine.isValid()){
        machine.sort();
    }else{
        qWarning() << "InstrumentReader::getInstrument: Data::Instrument object of id " << id << "is invalid. Is instrument.ini complete?";
    }

    return machine;
}

/*
Unallocates memory of all loaded data
*/
void InstrumentReader::unload(){
    std::vector<InstrumentID>().swap(this->instrument_ids);
    data_loaded = false;
}

/*
Returns whether Data::InstrumentReader is valid (has loaded cytometer data)
    :returns: validity
*/
bool InstrumentReader::isValid() const {
    return this->data_loaded;
}

} // Data namespace
