/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
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
** :class: DataFactory
** A factory that checks existance of data paths. If path is invalid, will fatal
** error upon data object request
** 
** :class: DataError
** A QMessageBox to notify the user of a fatal data error
** 
** :class: DataWarning
** A QMessageBox to warn the user for missing data
** 
***************************************************************************/

#ifndef DATA_FACTORY_H
#define DATA_FACTORY_H

#include "data_global.h"

#include <memory>

#include <QString>
#include <QSettings>
#include <QMessageBox>

class DATALIB_EXPORT DataFactory {
    public:
        DataFactory();
        DataFactory(const QString settings, const QString styles, const QString cytometers, const QString fluorophores);
        DataFactory(const DataFactory&) = default;         // Copy/Move - not sure whether you would want to, but should be fine
        DataFactory& operator=(const DataFactory&) = default;
        DataFactory(DataFactory&&) = default;
        DataFactory& operator=(DataFactory&&) = default;
        ~DataFactory() = default;

        bool isValid() const;
        bool isWarning() const;

        void execMessages() const;

        QString getPathSettings() const;
        QString getPathDefaults() const;
        QString getPathStyles() const;
        QString getPathCytometers() const;
        QString getPathFluorophores() const;
        std::unique_ptr<QSettings> get(const QString& settings) const;

    private:
        const QString file_settings;
        const QString file_styles;
        const QString file_cytometers;
        const QString file_fluorophores;
        const QString path_exe;

        QString path_settings;
        QString path_defaults;
        QString path_cytometers;
        QString path_fluorophores;
        QString path_styles;

        bool valid_settings;
        bool valid_defaults;
        bool valid_styles;
        bool valid_cytometers;
        bool valid_fluorophores;

        bool error_fatal;
        bool error_warning;
    
        static bool exists(const QString& path_file);
};

class DATALIB_EXPORT DataError : public QMessageBox {
    public:
        DataError();
        DataError(const QString& message);
        DataError(const DataError&) = delete;
        DataError& operator=(const DataError&) = delete;
        DataError(DataError&&) = delete;
        DataError& operator=(DataError&&) = delete;
        ~DataError() = default;
};

class DATALIB_EXPORT DataWarning : public QMessageBox {
    public:
        DataWarning();
        DataWarning(const QString& message);
        DataWarning(const DataWarning&) = delete;
        DataWarning& operator=(const DataWarning&) = delete;
        DataWarning(DataWarning&&) = delete;
        DataWarning& operator=(DataWarning&&) = delete;
        ~DataWarning() = default;
};  

#endif // DATA_FACTORY_H