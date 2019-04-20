/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_fluorophores.h"

#include <QDebug>

/*
Constructor: Construct the fluorophore data types
*/
DataFluorophores::DataFluorophores() :
    fluor_name(),
    fluor_id()
{}

/*
Loads the fluorophores QSettings
Note: do not call this if DataFactory 'fluorophores' is invalid, that causes the factory to qFatal()
    :param data: the DataFactory to request the source data from
*/
void DataFluorophores::load(DataFactory& data){
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores;
    fluorophores = data.get("fluorophores");

    // Get default parameters
    QString default_enable;

    default_enable = fluorophores->value("DEFAULT/enable", true).toBool();

    // Reserving memory to prevent unnessary growing
    unsigned int data_size;
    unsigned int multi_size;
    data_size = fluorophores->allKeys().size();
    multi_size = 5; // A guess, depends on amount of alternative names, if vector/unordered_map is too small, will auto grow
    this->fluor_id.reserve(data_size + multi_size);
    this->fluor_name.reserve(data_size + multi_size);

    // Iterate over data file to load all data objects
    for(const QString &group : fluorophores->childGroups()){
        if(group == "DEFAULT"){
            continue;
        }

        fluorophores->beginGroup(group);

        if(!fluorophores->value("enable", default_enable).toBool()){
            fluorophores->endGroup();
            continue;
        }

        // Builds list of names for menu's etc.
        QStringList group_names{fluorophores->value("names", group).toStringList()};
        this->fluor_name.insert(std::end(this->fluor_name), std::begin(group_names), std::end(group_names));          

        // Build fluorophore name to fluorophore ID lookup map
        for(const QString &name : group_names){
            // Add fluorophore name corresponding fluorophore ID for lookup
            this->fluor_id[name] = group;
        }

        fluorophores->endGroup();
    }

    // Sort alphabetical order
    std::sort(this->fluor_name.begin(), this->fluor_name.end());
    
    // Sets the loaded flag
    data_loaded = true;
}

/*
Unallocates memory of all loaded data
*/
void DataFluorophores::unload(){
    std::unordered_map<QString, QString>().swap(this->fluor_id);
    std::vector<QString>().swap(this->fluor_name);
    data_loaded = false;
}

/*
Returns whether DataFluorophores is valid (has loaded fluorophore data)
    :returns: validity
*/
bool DataFluorophores::isValid() const {
    if(this->data_loaded){
        return true;
    }else{
        return false;
    }
}

/*
Fluorophore spectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param id: the fluorophore id, if id is invalid returns are empty vectors
    :returns: DataSpectrum object
*/
std::unique_ptr<DataSpectrum> DataFluorophores::getSpectrum(const DataFactory& data, const QString& id) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores;
    fluorophores = data.get("fluorophores");

    fluorophores->beginGroup(id);

    // Load data and transform to vector of double
    std::vector<double> excitation_wavelength{DataFluorophores::toStdVector(fluorophores->value("excitation_wavelength", "0.0").toStringList())};
    std::vector<double> excitation_intensity{DataFluorophores::toStdVector(fluorophores->value("excitation_intensity", "0.0").toStringList())};
    std::vector<double> emission_wavelength{DataFluorophores::toStdVector(fluorophores->value("emission_wavelength", "0.0").toStringList())};
    std::vector<double> emission_intensity{DataFluorophores::toStdVector(fluorophores->value("emission_intensity", "0.0").toStringList())};

    fluorophores->endGroup();

    // Check if valid -> if not send qWarning and build a new (empty) DataSpectrum
    std::unique_ptr<DataSpectrum> spectrum;
    spectrum = std::make_unique<DataSpectrum>(id, excitation_wavelength, excitation_intensity, emission_wavelength, emission_intensity);

    // Verify validity, if invalid return
    if(spectrum->isValid()){
        return spectrum;
    }else{
        std::vector<double> vector_null(1);
        vector_null = {0.0};
        
        qWarning() << "DataFluorophores::getSpectrum: DataSpectrum object of id " << id << " is invalid.";
        spectrum = std::make_unique<DataSpectrum>(id, vector_null, vector_null, vector_null, vector_null);
        return spectrum;
    }
}

/*
Fluorophore CacheSpectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param index: the creation index
    :param id: the fluorophore id, if id is invalid returns are empty vectors
    :returns: DataSpectrum object
*/
std::unique_ptr<CacheSpectrum> DataFluorophores::getCacheSpectrum(const DataFactory& data, unsigned int index, const QString& id, const QString& name) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores;
    fluorophores = data.get("fluorophores");

    fluorophores->beginGroup(id);

    // Load data and transform to vector of double
    std::vector<double> excitation_wavelength{DataFluorophores::toStdVector(fluorophores->value("excitation_wavelength", "0.0").toStringList())};
    std::vector<double> excitation_intensity{DataFluorophores::toStdVector(fluorophores->value("excitation_intensity", "0.0").toStringList())};
    std::vector<double> emission_wavelength{DataFluorophores::toStdVector(fluorophores->value("emission_wavelength", "0.0").toStringList())};
    std::vector<double> emission_intensity{DataFluorophores::toStdVector(fluorophores->value("emission_intensity", "0.0").toStringList())};

    DataMeta meta;
    meta.excitation_max = fluorophores->value("excitation_max", -1).toInt();
    meta.emission_max = fluorophores->value("emission_max", -1).toInt();

    QStringList fluor_names = fluorophores->value("names", id).toStringList();

    fluorophores->endGroup();

    // Verify validity, if invalid return
    DataSpectrum spectrum{id, excitation_wavelength, excitation_intensity, emission_wavelength, emission_intensity};
    if(!spectrum.isValid()){
        std::vector<double> vector_null(1);
        vector_null = {0.0};
        
        qWarning() << "DataFluorophores::getSpectrum: DataSpectrum object of id " << id << " is invalid.";
        DataSpectrum spectrum{id, vector_null, vector_null, vector_null, vector_null};
    }
    
    std::unique_ptr<CacheSpectrum> cache;
    cache = std::make_unique<CacheSpectrum>(index, name, std::move(fluor_names), std::move(spectrum), std::move(meta));

    return(cache);
}

/*
Getter for the ordered fluorophore name vector
    :returns: reference to fluor_name
*/
const std::vector<QString>& DataFluorophores::getFluorName() const {
    return this->fluor_name;
}

/*
Getter for the fluorophore id map
    :returns: reference to fluor_id
*/
const std::unordered_map<QString, QString>& DataFluorophores::getFluorID() const {
    return this->fluor_id;
}

/*
(Static) Converts a QStringList into a std::vector of doubles
    :returns: vector of double typeconverted QStrings
*/
std::vector<double> DataFluorophores::toStdVector(const QStringList& stringlist){
    std::vector<double> double_vector(stringlist.size());

    // Iterate using index, because I need to read the input to the output at the same time   
    for(int i=0; i<stringlist.size(); ++i){
        double_vector[i] = stringlist[i].toDouble();
    }

    return double_vector;
} 

/*
(Static) Debugging functions, unpacks Map/Set for parsing to qDebug()
*/
void DataFluorophores::qDebugMap(const std::unordered_map<QString, QString>& map){
    qDebug() << "std::unordered_map<QString, QString>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void DataFluorophores::qDebugMap(const std::unordered_map<QString, QStringList>& map){
    qDebug() << "std::unordered_map<QString, QStringList>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void DataFluorophores::qDebugMap(const std::unordered_map<QString, std::vector<int>>& map){
    qDebug() << "std::unordered_map<QString, std::vector<int>>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void DataFluorophores::qDebugSet(const std::unordered_set<QString>& map){
    qDebug() << "std::unordered_set<QString>:";
    for(const QString &key: map){
        qDebug() << "{" << key << "}";
    }
}
