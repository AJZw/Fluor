/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "main_menu_bar.h"

#include <QAction>
#include <QIcon>
#include <QKeySequence>

namespace Main {

/*
Constructor: constructs the file menu
  :param parent: the parent widget
*/
FileMenu::FileMenu(QWidget* parent) : QMenu(parent) {
  this->setTitle("&File");
  this->setIcon(QIcon());

  this->setToolTipsVisible(false);
  this->setTearOffEnabled(false);
  this->setSeparatorsCollapsible(false);

  // Populate menu
  QAction* action_save = new QAction("&Save As...", this);
  action_save->setCheckable(false);
  action_save->setEnabled(false);
  action_save->setShortcut(QKeySequence::Save);
  QObject::connect(action_save, &QAction::triggered, this, &FileMenu::triggered_saveas);

  QAction* action_open = new QAction("&Open...", this);
  action_open->setCheckable(false);
  action_open->setEnabled(false);
  action_open->setShortcut(QKeySequence::Open);
  QObject::connect(action_open, &QAction::triggered, this, &FileMenu::triggered_open);

  QAction* action_print = new QAction("&Print", this);
  action_print->setCheckable(false);
  action_print->setEnabled(false);
  action_print->setShortcut(QKeySequence::Print);
  QObject::connect(action_print, &QAction::triggered, this, &FileMenu::triggered_print);

  QAction* action_exit = new QAction("&Exit", this);
  action_exit->setCheckable(false);
  action_exit->setEnabled(true);
  action_exit->setShortcut(QKeySequence::Quit);
  QObject::connect(action_exit, &QAction::triggered, this, &FileMenu::triggered_exit);

  this->addAction(action_save);
  this->addAction(action_open);
  this->addSeparator();
  this->addAction(action_print);
  this->addSeparator();
  this->addAction(action_exit);
}

/*
Slot: receives triggered signal of the 'save as' action
*/
void FileMenu::triggered_saveas(bool checked) {
  Q_UNUSED(checked);
  emit this->sendAction(Main::MenuBarAction::SaveAs, QVariant());
}

/*
Slot: receives triggered signal of the 'open' action
*/
void FileMenu::triggered_open(bool checked) {
  Q_UNUSED(checked);
  emit this->sendAction(Main::MenuBarAction::Open, QVariant());
}

/*
Slot: receives triggered signal of the 'print' action
*/
void FileMenu::triggered_print(bool checked) {
  Q_UNUSED(checked);
  emit this->sendAction(Main::MenuBarAction::Print, QVariant());
}

/*
Slot: receives triggered signal of the 'exit' action
*/
void FileMenu::triggered_exit(bool checked) {
  Q_UNUSED(checked);
  emit this->sendAction(Main::MenuBarAction::Exit, QVariant());
}

// #################################################################################### //

/*
Constructor: constructs the file menu
  :param parent: the parent widget
*/
InstrumentMenu::InstrumentMenu(QWidget* parent) : QMenu(parent) {
  this->setTitle("&Instrument");
  this->setIcon(QIcon());

  this->setToolTipsVisible(false);
  this->setTearOffEnabled(false);
  this->setSeparatorsCollapsible(false);

  this->buildMenu({});
  QObject::connect(this, &QMenu::triggered, this, &InstrumentMenu::triggered_instrument);
}

/*
Selects the specified instrument
  :param id: the instrument to select
*/
void InstrumentMenu::selectInstrument(QString id) {
  // First deselect all
  for (auto* action : this->actions()) {
    action->setChecked(false);
  }

  // Find entree
  for (auto* action : this->actions()) {
    if (action->data().toString() == id) {
      action->setChecked(true);
      break;
    }
  }
}

/*
Fills the menu with the instrument names
*/
void InstrumentMenu::buildMenu(const Data::InstrumentReader& data) {
  // Clear the menu, removing also deletes owning QActions
  this->clear();

  QActionGroup* instrument_actions = new QActionGroup(this);
  instrument_actions->setExclusive(true);

  QAction* action_none = new QAction("&None", instrument_actions);
  action_none->setData("");
  action_none->setCheckable(true);

  for (const auto& id : data.getInstruments()) {
    QAction* action_instrument = new QAction(id.name, instrument_actions);
    action_instrument->setData(id.id);
    action_instrument->setCheckable(true);
  }
  this->addActions(instrument_actions->actions());
}

/*
Slot: receives triggered signal of the instrument selection
*/
void InstrumentMenu::triggered_instrument(QAction* action) {
  QString id = QString();
  if (action->isChecked()) {
    id = action->data().toString();
  }

  emit this->sendAction(Main::MenuBarAction::InstrumentID, id);
}

/*
Slot: receives the instrument id to be selected
*/
void InstrumentMenu::receiveInstrument(QString id) { this->selectInstrument(id); }

/*
Slot: receives instrument identifiers
*/
void InstrumentMenu::receiveInstruments(const Data::InstrumentReader& data) { this->buildMenu(data); }

// #################################################################################### //

/*
Constructor: constructs the file menu
  :param parent: the parent widget
*/
OptionsMenu::OptionsMenu(QWidget* parent) : QMenu(parent), menu_sorting(nullptr), menu_styles(nullptr) {
  this->setTitle("&Options");
  this->setIcon(QIcon());

  this->setToolTipsVisible(false);
  this->setTearOffEnabled(false);
  this->setSeparatorsCollapsible(false);

  // Build and populate sorting menu
  this->menu_sorting = this->addMenu("&Sorting");
  QObject::connect(this->menu_sorting, &QMenu::triggered, this, &OptionsMenu::triggered_sorting);

  QActionGroup* sort_actions = new QActionGroup(menu_sorting);
  sort_actions->setExclusive(true);
  // The code assumes the following hard-coded values! if order is changed make sure to update rest of the code
  const std::vector<QString> sort_options = {"Additive", "Alphabetical", "Excitation", "Emission"};
  for (const auto& option : sort_options) {
    QAction* action = new QAction(option, sort_actions);
    action->setCheckable(true);
  }
  this->menu_sorting->addActions(sort_actions->actions());

  this->menu_sorting->addSeparator();

  QAction* action_reverse = new QAction("Reversed", this->menu_sorting);
  action_reverse->setCheckable(true);
  menu_sorting->addAction(action_reverse);

  // Build styles menu
  this->menu_styles = this->addMenu("&Styles");
  QObject::connect(this->menu_styles, &QMenu::triggered, this, &OptionsMenu::triggered_styles);
}

/*
Slot: receives triggered signal of the style menu
*/
void OptionsMenu::triggered_styles(QAction* action) {
  QString id = QString();
  if (action->isChecked()) {
    id = action->data().toString();
  }

  emit this->sendAction(Main::MenuBarAction::StyleID, id);
}

/*
Slot: receives triggered signal from the sorting menu
*/
void OptionsMenu::triggered_sorting(QAction* action) {
  // Get index
  int index;
  for (index = 0; index < this->menu_sorting->actions().length(); ++index) {
    if (action == this->menu_sorting->actions()[index]) {
      break;
    }
  }

  if (index == 0) {
    // Additive
    if (this->menu_sorting->actions()[5]->isChecked()) {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::AdditiveReversed));
    } else {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Additive));
    }
  } else if (index == 1) {
    // Alphabetical
    if (this->menu_sorting->actions()[5]->isChecked()) {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::AlphabeticalReversed));
    } else {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Alphabetical));
    }
  } else if (index == 2) {
    // Excitation
    if (this->menu_sorting->actions()[5]->isChecked()) {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::ExcitationReversed));
    } else {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Excitation));
    }
  } else if (index == 3) {
    // Emission
    if (this->menu_sorting->actions()[5]->isChecked()) {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::EmissionReversed));
    } else {
      emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Emission));
    }
  } else if (index == 5) {
    // Reverse
    // Find checked sorting mode
    if (this->menu_sorting->actions()[0]->isChecked()) {
      if (action->isChecked()) {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::AdditiveReversed));
      } else {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Additive));
      }
    } else if (this->menu_sorting->actions()[1]->isChecked()) {
      if (action->isChecked()) {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::AlphabeticalReversed));
      } else {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Alphabetical));
      }
    } else if (this->menu_sorting->actions()[2]->isChecked()) {
      if (action->isChecked()) {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::ExcitationReversed));
      } else {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Excitation));
      }
    } else if (this->menu_sorting->actions()[3]->isChecked()) {
      if (action->isChecked()) {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::EmissionReversed));
      } else {
        emit this->sendAction(Main::MenuBarAction::SortOrder, static_cast<int>(State::SortMode::Emission));
      }
    } else {
      qWarning() << "OptionsMenu::triggered_sorting: no checked sorting mode";
    }
  } else {
    qWarning() << "OptionsMenu::triggered_sorting: unknown action index";
  }
}

/*
Select the specified style
  :param id: the style identifier
*/
void OptionsMenu::selectStyle(const QString& id) {
  // Find entree
  for (auto* action : this->menu_styles->actions()) {
    if (action->data().toString() == id) {
      action->setChecked(true);
      break;
    }
  }
}

/*
Builds the list of style identifiers for the styles submenu
  :param styles: the list of style identifiers
*/
void OptionsMenu::buildStyles(const std::vector<Data::StyleID>& styles) {
  this->menu_styles->clear();

  QActionGroup* style_options = new QActionGroup(this->menu_styles);
  for (const auto& id : styles) {
    QAction* action_style = new QAction(id.name, style_options);
    action_style->setData(id.id);
    action_style->setCheckable(true);
  }
  this->menu_styles->addActions(style_options->actions());
}

/*
Selects the proper actions based on the sorting mode
*/
void OptionsMenu::selectSortMode(State::SortMode mode) {
  switch (mode) {
    case State::SortMode::Additive: {
      this->menu_sorting->actions()[0]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(false);
      break;
    }
    case State::SortMode::AdditiveReversed: {
      this->menu_sorting->actions()[0]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(true);
      break;
    }
    case State::SortMode::Alphabetical: {
      this->menu_sorting->actions()[1]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(false);
      break;
    }
    case State::SortMode::AlphabeticalReversed: {
      this->menu_sorting->actions()[1]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(true);
      break;
    }
    case State::SortMode::Excitation: {
      this->menu_sorting->actions()[2]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(false);
      break;
    }
    case State::SortMode::ExcitationReversed: {
      this->menu_sorting->actions()[2]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(true);
      break;
    }
    case State::SortMode::Emission: {
      this->menu_sorting->actions()[3]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(false);
      break;
    }
    case State::SortMode::EmissionReversed: {
      this->menu_sorting->actions()[3]->setChecked(true);
      this->menu_sorting->actions()[5]->setChecked(true);
      break;
    }
    default:
      break;
  }
}

/*
Slot: receives the signal to select a specific style
*/
void OptionsMenu::receiveStyle(const QString& id) { this->selectStyle(id); }

/*
Slot: receives styles identifier information
*/
void OptionsMenu::receiveStyles(const std::vector<Data::StyleID>& styles) { this->buildStyles(styles); }

/*
Slot: receive sorting option
*/
void OptionsMenu::receiveSortMode(State::SortMode mode) { this->selectSortMode(mode); }

// #################################################################################### //

/*
Constructor: constructs the file menu
  :param parent: the parent widget
*/
HelpMenu::HelpMenu(QWidget* parent) : QMenu(parent) {
  this->setTitle("&Help");
  this->setIcon(QIcon());

  this->setToolTipsVisible(false);
  this->setTearOffEnabled(false);
  this->setSeparatorsCollapsible(false);

  QAction* action_about = new QAction("&About Fluor", this);
  action_about->setCheckable(false);
  QObject::connect(action_about, &QAction::triggered, this, &HelpMenu::triggered_about);
  this->addAction(action_about);
}

/*
Slot: receives 'about fluor' signal
*/
void HelpMenu::triggered_about(bool checked) {
  Q_UNUSED(checked);
  emit this->sendAction(Main::MenuBarAction::About, QVariant());
}

// #################################################################################### //

/*
Constructor: construct a menubar
  :param parent: the parent widget (keep nullptr for program wide menubars on Mac!)
*/
MenuBar::MenuBar(QWidget* parent)
    : QMenuBar(parent), file_menu(nullptr), instrument_menu(nullptr), options_menu(nullptr), help_menu(nullptr) {
  this->setNativeMenuBar(true);

  // Instantiate the menus, and build connections
  file_menu = new FileMenu(this);
  instrument_menu = new InstrumentMenu(this);
  options_menu = new OptionsMenu(this);
  help_menu = new HelpMenu(this);

  QObject::connect(file_menu, &Main::FileMenu::sendAction, this, &Main::MenuBar::receiveMenuBarStateChange);

  QObject::connect(this, &Main::MenuBar::sendInstruments, instrument_menu, &Main::InstrumentMenu::receiveInstruments);
  QObject::connect(this, &Main::MenuBar::sendInstrument, instrument_menu, &Main::InstrumentMenu::receiveInstrument);
  QObject::connect(instrument_menu, &Main::InstrumentMenu::sendAction, this, &Main::MenuBar::receiveMenuBarStateChange);

  QObject::connect(this, &Main::MenuBar::sendStyles, options_menu, &Main::OptionsMenu::receiveStyles);
  QObject::connect(this, &Main::MenuBar::sendStyle, options_menu, &Main::OptionsMenu::receiveStyle);
  QObject::connect(this, &Main::MenuBar::sendSortMode, options_menu, &Main::OptionsMenu::receiveSortMode);
  QObject::connect(options_menu, &Main::OptionsMenu::sendAction, this, &Main::MenuBar::receiveMenuBarStateChange);

  QObject::connect(help_menu, &Main::HelpMenu::sendAction, this, &Main::MenuBar::receiveMenuBarStateChange);

  // Add menu's to the menubar
  this->addMenu(file_menu);
  this->addMenu(instrument_menu);
  this->addMenu(options_menu);
  this->addMenu(help_menu);
}

/*
Slot: recieves menubar state updates from the State::Program
*/
void MenuBar::receiveMenuBarStateUpdate(Main::MenuBarAction action, const QVariant& id) {
  switch (action) {
    case Main::MenuBarAction::InstrumentID: {
      emit this->sendInstrument(id.toString());
      break;
    }
    case Main::MenuBarAction::StyleID: {
      emit this->sendStyle(id.toString());
      break;
    }
    case Main::MenuBarAction::SortOrder: {
      emit this->sendSortMode(static_cast<State::SortMode>(id.toInt()));
      break;
    }
    case Main::MenuBarAction::SaveAs:
    case Main::MenuBarAction::Open:
    case Main::MenuBarAction::Print:
    case Main::MenuBarAction::Exit:
    case Main::MenuBarAction::About:
    default:
      break;
  }
}

/*
Slot: receives the menubar state changes from the QMenu's
*/
void MenuBar::receiveMenuBarStateChange(Main::MenuBarAction action, const QVariant& id) { emit this->sendMenuBarStateChange(action, id); }

/*
Slot: receives instruments identifiers
*/
void MenuBar::receiveInstruments(const Data::InstrumentReader& instruments) { emit this->sendInstruments(instruments); }

/*
Slot: receives styles identifiers
*/
void MenuBar::receiveStyles(const std::vector<Data::StyleID>& styles) { emit this->sendStyles(styles); }

}  // namespace Main
