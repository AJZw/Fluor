/**** General **************************************************************
** Version:    v0.9.11
** Date:       2020-10-27
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** data_factory.h is part of Fluor
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
** Settings and data file parsers
**
** :class: Data::Factory
** A factory that checks existance of data paths. If path is invalid, will fatal
** error upon data object request
**
** :class: Data::Error
** A QMessageBox to notify the user of a fatal data error
**
** :class: Data::Warning
** A QMessageBox to warn the user for missing data
**
***************************************************************************/

#ifndef DATA_FACTORY_H
#define DATA_FACTORY_H

#include <QJsonDocument>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <memory>

#include "data_global.h"

namespace Data {

class DATALIB_EXPORT Factory {
 public:
  Factory();
  Factory(const QString settings, const QString styles, const QString cytometers, const QString fluorophores);
  Factory(const Factory&) = default;
  // Copy/Move constructor - not sure whether useful, but should be fine because this class doesnt do any file IO
  Factory& operator=(const Factory&) = default;
  Factory(Factory&&) = default;
  Factory& operator=(Factory&&) = default;
  ~Factory() = default;

  enum type { Settings, Defaults, Styles, Instruments, Fluorophores };

  bool isValid() const;
  bool isValid(Factory::type type) const;
  bool isWarning() const;

  void execMessages() const;

  QString getPathSettings() const;
  QString getPathDefaults() const;
  QString getPathStyles() const;
  QString getPathInstruments() const;
  QString getPathFluorophores() const;
  std::unique_ptr<QSettings> get(const Factory::type type) const;
  QJsonDocument get_json(const Factory::type type) const;

 private:
  const QString file_settings;
  const QString file_styles;
  const QString file_instruments;
  const QString file_fluorophores;
  const QString path_exe;

  QString path_settings;
  QString path_defaults;
  QString path_instruments;
  QString path_fluorophores;
  QString path_styles;

  bool valid_settings;
  bool valid_defaults;
  bool valid_styles;
  bool valid_instruments;
  bool valid_fluorophores;

  bool error_fatal;
  bool error_warning;

  static bool exists(const QString& path_file);
};

class DATALIB_EXPORT Error : public QMessageBox {
 public:
  Error();
  Error(const QString& message);
  Error(const Error&) = delete;
  Error& operator=(const Error&) = delete;
  Error(Error&&) = delete;
  Error& operator=(Error&&) = delete;
  ~Error() = default;
};

class DATALIB_EXPORT Warning : public QMessageBox {
 public:
  Warning();
  Warning(const QString& message);
  Warning(const Warning&) = delete;
  Warning& operator=(const Warning&) = delete;
  Warning(Warning&&) = delete;
  Warning& operator=(Warning&&) = delete;
  ~Warning() = default;
};

}  // namespace Data

#endif  // DATA_FACTORY_H