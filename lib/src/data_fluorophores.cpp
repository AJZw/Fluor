/**** General **************************************************************
** Version:    v0.10.1
** Date:       2020-11-16
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_fluorophores.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QPolygonF>

namespace Data {

/*
Constructor: Construct the fluorophore data types
*/
FluorophoreReader::FluorophoreReader() : fluor_data(), fluor_name(), fluor_id(), fluor_names() {}

/*
Loads the fluorophores QJsonDocument
Note: do not call this if Data::Factory::type::Fluorophores is invalid, that causes the factory to qFatal()
  :param data: the Data::Factory to request the source data from
*/
void FluorophoreReader::load(Data::Factory& factory) {
  // Retrieve QSetting
  this->fluor_data = factory.get_json(Data::Factory::Fluorophores);

  // Clear ids just in case load is called sequentially without unloading first
  this->fluor_id.clear();
  this->fluor_name.clear();
  this->fluor_names.clear();

  if (!this->isValid()) {
    return;
  }

  // Reserve the necessary vector size
  QJsonObject data = this->fluor_data.object();

  // Reserving memory to prevent unnessary growing
  std::size_t data_size = static_cast<std::size_t>(data.length());
  std::size_t multi_size =
      data_size / 4;  // A guess, depends on amount of alternative names, if vector/unordered_map is too small, will auto grow anyway

  this->fluor_id.reserve(data_size + multi_size);
  this->fluor_name.reserve(data_size + multi_size);
  this->fluor_names.reserve(multi_size);

  // Iterate over data file to load all data objects
  for (const QString& group : data.keys()) {
    QJsonObject fluorophore = data[group].toObject();

    // Skip disabled fluorophores
    if (!fluorophore["enable"].toBool(true)) {
      continue;
    }

    // Builds list of names for menu's etc.
    QJsonArray fluorophore_names = fluorophore["names"].toArray();

    // Build fluorophore names map
    QStringList names_stringlist = QStringList();
    names_stringlist.reserve(fluorophore_names.size());

    // Fill vectors and maps
    for (const QJsonValueRef name_ref : fluorophore_names) {
      QString name = name_ref.toString();
      // Add to name list
      this->fluor_name.push_back(name);
      // Add to id lookup
      this->fluor_id[name] = group;
      // Add name to alternative name lookup
      names_stringlist.append(name);
    }

    // Finally add the alternative name stringlist to the storage vector
    for (const QJsonValueRef name : fluorophore_names) {
      this->fluor_names[name.toString()] = names_stringlist;
    }
  }

  // Sort case-insensitive alphabetical order
  std::sort(this->fluor_name.begin(), this->fluor_name.end(),
            [](const QString& left, const QString& right) { return left.toLower() < right.toLower(); });
}

/*
Unallocates memory of all loaded data
*/
void FluorophoreReader::unload() {
  this->fluor_data = QJsonDocument();
  std::vector<QString>().swap(this->fluor_name);
  std::unordered_map<QString, QString>().swap(this->fluor_id);
  std::unordered_map<QString, QStringList>().swap(this->fluor_names);
}

/*
Returns whether Fluorophores is valid (has loaded fluorophore data)
  :returns: validity
*/
bool FluorophoreReader::isValid() const { return !this->fluor_data.isNull(); }

/*
Fluorophore spectrum accessor
  :param id: the fluorophore id, if id is not in source data, returns valid but otherwise useless data
*/
Data::Spectrum FluorophoreReader::getSpectrum(const QString& id) const {
  // Retrieve data
  QJsonObject data_object = this->fluor_data.object();
  QJsonValueRef data_ref = data_object[id];

  if (data_ref.type() == QJsonValue::Type::Null || data_ref.type() == QJsonValue::Type::Undefined) {
    qWarning() << "InstrumentReader::getSpectrum: Data::Spectrum object of id" << id << "could not be found.";
  }

  QJsonObject data = data_ref.toObject();

  // Load data and transform to Data::Polygon's
  bool is_absorption = false;
  QJsonArray excitation_wavelength = data["excitation_wavelength"].toArray();
  QJsonArray excitation_intensity = data["excitation_intensity"].toArray();

  // If no excitation is available fallback to absorption
  if (excitation_wavelength.empty() || excitation_intensity.empty()) {
    is_absorption = true;
    excitation_wavelength = data["absorption_wavelength"].toArray();
    excitation_intensity = data["absorption_intensity"].toArray();
  }
  Data::Polygon excitation = this->toPolygon(excitation_wavelength, excitation_intensity);

  QJsonArray emission_wavelength = data["emission_wavelength"].toArray();
  QJsonArray emission_intensity = data["emission_intensity"].toArray();
  Data::Polygon emission = this->toPolygon(emission_wavelength, emission_intensity);

  // Set linecolor, cannot use the meta-data shortcut as spectrum doesnt request this info
  emission.setColor();
  excitation.setColor(emission.color());

  // Verify validity, if invalid return
  Data::Spectrum spectrum(id, excitation, emission);
  spectrum.setAbsorptionFlag(is_absorption);

  if (!spectrum.isValid()) {
    qWarning() << "Data::FluorophoreReader::getSpectrum: Data::Spectrum object of id " << id << " is invalid. Is the data file complete?";
  }

  return spectrum;
}

/*
Fluorophore CacheSpectrum accessor
  :param id: the fluorophore id, if id is not in source data, returns valid but otherwise useless data
  :param index: the creation index
  :returns: CacheSpectrum object
*/
Data::CacheSpectrum FluorophoreReader::getCacheSpectrum(const QString& id, unsigned int index) const {
  // Retrieve data
  QJsonObject data_object = this->fluor_data.object();
  QJsonValueRef data_ref = data_object[id];

  if (data_ref.type() == QJsonValue::Type::Null || data_ref.type() == QJsonValue::Type::Undefined) {
    qWarning() << "InstrumentReader::getSpectrum: Data::Spectrum object of id" << id << "could not be found.";
  }

  QJsonObject data = data_ref.toObject();
  // Load data and transform to Data::Polygon's
  bool is_absorption = false;
  double excitation_max = data["excitation_max"].toDouble(-1);
  QJsonArray excitation_wavelength = data["excitation_wavelength"].toArray();
  QJsonArray excitation_intensity = data["excitation_intensity"].toArray();

  // If no excitation is available fallback to absorption
  if (excitation_wavelength.empty() || excitation_intensity.empty()) {
    is_absorption = true;
    excitation_max = data["absorption_max"].toDouble(-1);
    excitation_wavelength = data["absorption_wavelength"].toArray();
    excitation_intensity = data["absorption_intensity"].toArray();
  }
  Data::Polygon excitation = this->toPolygon(excitation_wavelength, excitation_intensity);

  double emission_max = data["emission_max"].toDouble(-1);
  QJsonArray emission_wavelength = data["emission_wavelength"].toArray();
  QJsonArray emission_intensity = data["emission_intensity"].toArray();
  Data::Polygon emission = this->toPolygon(emission_wavelength, emission_intensity);

  // Get meta data
  Data::Meta meta;
  meta.excitation_max = excitation_max;
  meta.emission_max = emission_max;

  // Set linecolor
  if (meta.emission_max != -1) {
    emission.setColor(meta.emission_max);
  } else {
    emission.setColor();
  }
  excitation.setColor(emission.color());

  // Verify validity, if invalid return
  Data::Spectrum spectrum(id, excitation, emission);
  spectrum.setAbsorptionFlag(is_absorption);

  if (!spectrum.isValid()) {
    qWarning() << "Data::FluorophoreReader::getCacheSpectrum: DataSpectrum object of id" << id << "is invalid. Is the data file complete?";
  }

  // Wrap in cache
  Data::CacheSpectrum cache(index, spectrum, meta);
  return cache;
}

/*
Getter for the ordered fluorophore name vector
  :returns: reference to fluor_name
*/
const std::vector<QString>& FluorophoreReader::getFluorName() const { return this->fluor_name; }

/*
Getter for the fluorophore id map
  :returns: reference to fluor_id
*/
const std::unordered_map<QString, QString>& FluorophoreReader::getFluorID() const { return this->fluor_id; }

/*
Getter for the fluorophore names map
  :returns: reference to fluor_id
*/
const std::unordered_map<QString, QStringList>& FluorophoreReader::getFluorNames() const { return this->fluor_names; }

/*
(Static) Convers a QStringList into a Data::Polygon object.
  :param list_x: the list containing the values for the x-axis
  :param list_y: the list containing the values for the y-axis
  :returns: a Data::Polygon object containing the curve
*/
Data::Polygon FluorophoreReader::toPolygon(const QStringList& list_x, const QStringList& list_y) {
  if (list_x.size() != list_y.size()) {
    qWarning() << "Fluorophores::toPolygon: x and y stringlist are of unequal size, cannot be parsed, returns default Data::Polygon";
    return Data::Polygon();
  }
  if (list_x.size() < 2) {
    qFatal("Fluorophores::toPolygon: x and y values should each consist of a list atleast two values");
  }

  // Reserve enough space, with two additional entrees, which can be used to make a properly closed polygon
  QPolygonF temp_polygon = QPolygonF();
  temp_polygon.reserve(list_x.size() + 2);
  temp_polygon.resize(list_x.size());

  // Get calculation temporaries
  double x_min = list_x[0].toDouble();
  double x_max = list_x[list_x.size() - 1].toDouble();

  double y_min = 0.0;
  double y_max = 100.0;

  // Fill temp QPolygonF
  for (int i = 0; i < list_x.size(); ++i) {
    double x = list_x[i].toDouble();
    double y = list_y[i].toDouble();

    // Set QPointF values
    temp_polygon[i].setX(x);
    temp_polygon[i].setY(y);
  }
  Data::Polygon polygon(x_min, x_max, y_min, y_max, std::move(temp_polygon));

  return polygon;
}

/*
(Static) Convers a QJsonArray into a Data::Polygon object.
  :param list_x: the array containing the values for the x-axis
  :param list_y: the array containing the values for the y-axis
  :returns: a Data::Polygon object containing the curve
*/
Data::Polygon FluorophoreReader::toPolygon(const QJsonArray& list_x, const QJsonArray& list_y) {
  if (list_x.size() != list_y.size()) {
    qWarning() << "Fluorophores::toPolygon: x and y QJsonArray's are of unequal size, cannot be parsed, returns default Data::Polygon";
    return Data::Polygon();
  }
  if (list_x.empty() || list_y.empty()) {
    qWarning() << "Fluorophores::toPolygon x and/or y QJsonArray is empty. Default Data::Polygon is returned";
    return Data::Polygon();
  }

  // Reserve enough space, with two additional entrees, which can be used to make a properly closed polygon
  QPolygonF temp_polygon = QPolygonF();
  temp_polygon.reserve(list_x.size() + 2);
  temp_polygon.resize(list_x.size());

  // Get polygon range
  double x_min = list_x[0].toDouble();
  double x_max = list_x[list_x.size() - 1].toDouble();

  double y_min = 0.0;
  double y_max = 100.0;

  // Fill temp QPolygonF
  for (int i = 0; i < list_x.size(); ++i) {
    double x = list_x[i].toDouble();
    double y = list_y[i].toDouble();

    // Set QPointF values
    temp_polygon[i].setX(x);
    temp_polygon[i].setY(y);
  }
  Data::Polygon polygon(x_min, x_max, y_min, y_max, std::move(temp_polygon));

  return polygon;
}

/*
(Static) Debugging functions, unpacks Map/Set for parsing to qDebug()
*/
void FluorophoreReader::qDebugMap(const std::unordered_map<QString, QString>& map) {
  qDebug() << "std::unordered_map<QString, QString>:";
  for (const auto& pair : map) {
    qDebug() << "{" << pair.first << ":" << pair.second << "}";
  }
}
void FluorophoreReader::qDebugMap(const std::unordered_map<QString, QStringList>& map) {
  qDebug() << "std::unordered_map<QString, QStringList>:";
  for (const auto& pair : map) {
    qDebug() << "{" << pair.first << ":" << pair.second << "}";
  }
}
void FluorophoreReader::qDebugMap(const std::unordered_map<QString, std::vector<double>>& map) {
  qDebug() << "std::unordered_map<QString, std::vector<int>>:";
  for (const auto& pair : map) {
    qDebug() << "{" << pair.first << ":" << pair.second << "}";
  }
}
void FluorophoreReader::qDebugSet(const std::unordered_set<QString>& map) {
  qDebug() << "std::unordered_set<QString>:";
  for (const QString& key : map) {
    qDebug() << "{" << key << "}";
  }
}

}  // namespace Data
