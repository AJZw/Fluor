/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** global.h is part of Fluor
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
** General purpose types that are used in multiple headers
** Namespaced to the main user of the enum
**
** :enum: State::ExcitationOption
** The excitation calculation mode
**
** :enum: State::SortMode
** The sorting method of the fluorophore buttons, mainly used by state and cache
**
** :enum: Bar::ButtonType
** Specifies a button of the Toolbar, mainly used by signal propagation from and to toolbar
**
***************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

namespace State {

enum class ExcitationOption { SinglePhoton, MultiPhoton };

enum class SortMode {
  Additive,
  AdditiveReversed,
  Alphabetical,
  AlphabeticalReversed,
  Excitation,
  ExcitationReversed,
  Emission,
  EmissionReversed
};

}  // namespace State

namespace Main {

enum class MenuBarAction { SaveAs, Open, Print, Exit, InstrumentID, SortOrder, StyleID, About };

}  // namespace Main

namespace Bar {

enum class ButtonType { Laser, Excitation, Emission, Filter, GraphAdd, GraphRemove, LaserLines };

}  // namespace Bar

#endif  // GLOBAL_H
