/**** General **************************************************************
** Version:    v0.9.12
** Date:       2020-10-28
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** data_spectrum.h is part of Fluor
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
** The spectrum objects
**
** :class: Data::Meta
** Struct for Spectrum meta data
**
** :class: Data::Polygon
** QPolygonF curve container
**
** :class: Data::Spectrum
** Container for excitation and emission curves (Data::Polygon) and ID
**
** :class: Data::CacheSpectrum
** Container for a Data::Spectrum and painting parameters
** 
***************************************************************************/

#ifndef DATA_SPECTRUM_H
#define DATA_SPECTRUM_H

#include "data_global.h"

#include <functional>

#include <QString>
#include <QStringList>
#include <QPolygonF>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QDebug>

namespace Data {

class DATALIB_EXPORT Meta {
    public:
        Meta();
        Meta(double excitation_max, double emission_max);
        double excitation_max;
        double emission_max;
};

class DATALIB_EXPORT Polygon {
    public:
        Polygon();
        Polygon(double x_min, double x_max, double y_min, double y_max, QPolygonF curve);
        Polygon(const Polygon& other);                  // Non default - forces deepcopy of QPolygonF
        Polygon& operator=(const Polygon& other);       // Non default - forces deepcopy of QPolygonF
        Polygon(Polygon&& other) = default;
        Polygon& operator=(Polygon&& other) = default;
        ~Polygon() = default;

        friend QDebug operator<<(QDebug stream, const Polygon& object){return stream << "Data::Polygon{" << object.x_min << "-" << object.x_max << ":" << object.curve[0] << "-" << object.curve[object.curve.size() - 1] << "}";};
        
        bool empty() const;
        double intensityAt(double wavelength, double cutoff=0.0) const;
        double intensityAtIter(double wavelength, double cutoff=0.0) const;
        double intensityMax() const;

        const QColor& color() const;
        void setColor();
        void setColor(double wavelength);
        void setColor(QColor color);
        static QColor visibleSpectrum(const double wavelength);

        QPolygonF& polygon();

        bool contains(const QPointF& point, double line_width) const;
        bool contains(const QPointF& point, double line_width, std::function<double(double)> scale_x) const;

        // Polygon scaling/moving - note: the cache is supposed to keep unmodified originals
        void scale(const Polygon& base, const QRectF& size, const double xg_start, const double xg_end, const double yg_start, const double yg_end, const double intensity=1.0);
        void scale(const Polygon& base, const QRectF& size, std::function<double(double)> scale_x, std::function<double(double, double)> scale_y, const double intensity=1.0);
        void copyCurve(const Polygon& base);
        void closeCurve(const QRectF& size);

    private:
        double x_min;   // in wavelength (nm)
        double x_max;   // in wavelength (nm)
        double y_min;   // in intensity (%)
        double y_max;   // in intensity (%)
        QColor curve_color;
        QPolygonF curve;
};

class DATALIB_EXPORT Spectrum {
    public:
        Spectrum(QString id);
        Spectrum(QString id, Polygon excitation=Polygon(), Polygon emission=Polygon());
        Spectrum(const Spectrum&) = default;
        Spectrum& operator=(const Spectrum&) = default;
        Spectrum(Spectrum&&) = default;
        Spectrum& operator=(Spectrum&&) = default;
        ~Spectrum() = default;

        friend QDebug operator<<(QDebug stream, const Spectrum& object){return stream << "{" << object.fluor_id << object.polygon_excitation << object.polygon_emission << "}";};

        QString id() const;
        bool isValid() const;

        const Data::Polygon& excitation() const;
        const Data::Polygon& emission() const;
        void setExcitation(Polygon polygon);
        void setEmission(Polygon polygon);

        bool absorptionFlag() const;
        bool twoPhotonFlag() const;
        void setAbsorptionFlag(bool flag);
        void setTwoPhotonFlag(bool flag);

        qreal excitationAt(double wavelength, double cutoff=0.0) const;
        qreal emissionAt(double wavelength, double cutoff=0.0) const;
        double excitationMax() const;
        double emissionMax() const;

    private:
        // Flags
        bool absorption;
        bool two_photon;

        // ID
        const QString fluor_id;

        // Line data
        Data::Polygon polygon_excitation;
        Data::Polygon polygon_emission;
};

// this class could inherit QObject to allow for signal/slot mechanics
class DATALIB_EXPORT CacheSpectrum {
    public:
        CacheSpectrum(unsigned int index, Data::Spectrum spectrum);
        CacheSpectrum(unsigned int index, Data::Spectrum spectrum, Data::Meta meta);
        CacheSpectrum(const CacheSpectrum&) = default;
        CacheSpectrum& operator=(const CacheSpectrum&) = default;
        CacheSpectrum(CacheSpectrum&&) = default;
        CacheSpectrum& operator=(CacheSpectrum&&) = default;
        ~CacheSpectrum() = default;

        friend QDebug operator<<(QDebug stream, const CacheSpectrum& object){return stream << object.spectrum_data;};

    private:
        // Index
        unsigned int cache_index;

        // Data
        const Data::Spectrum spectrum_data;
        const Data::Meta spectrum_meta;

        // General plotting parameters
        bool visible_excitation;
        bool visible_emission;

        bool select_excitation;
        bool select_emission;

        double intensity_cutoff;

        // Flags
        bool modified;

    public:
        QString id() const;

        unsigned int index() const;
        void setIndex(unsigned int index);

        double excitationMax() const;
        double emissionMax() const;

        bool visibleExcitation() const;
        bool visibleEmission() const;
        void setVisibleExcitation(bool visibility);
        void setVisibleEmission(bool visibility);

        bool selectExcitation() const;
        bool selectEmission() const;
        void setSelectExcitation(bool select);
        void setSelectEmission(bool select);

        double intensityCutoff() const;
        void setIntensityCutoff(const double cutoff);

        void resetModified();

        // Data::Spectrum getters, internally forwarded to the Data::Spectrum
        const Data::Spectrum& spectrum() const;
        Data::Spectrum copySpectrum() const;

        bool absorptionFlag() const;
        bool twoPhotonFlag() const;

        double excitationAt(double wavelength) const;
        double emissionAt(double wavelength) const;
};

} // Data namespace

#endif // DATA_SPECTRUM_H
