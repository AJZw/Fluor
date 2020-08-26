/**** General **************************************************************
** Version:    v0.9.8
** Date:       2020-08-05
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "application.h"

#include <QWidget>
#include <QSysInfo>
#include <QSettings>
#include <QIcon>
#include <QDebug>

Application::Application(int &argc, char **argv) :
    QApplication(argc, argv)
{
    this->setOrganizationName("AJZw");
    this->setApplicationName("Fluor");
    this->setApplicationVersion("v0.9.9");

    this->installEventFilter(this);

    // Set icon
    if(QSysInfo().productType() == "osx"){
        this->setWindowIcon(QIcon(":/icons/viewer_black.icns"));
    }else if(QSysInfo().productType() == "windows"){
        QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\DWM", QSettings::NativeFormat);
        unsigned int windows_accent = registry.value("AccentColor", 0xffffffff).toUInt();
        // int windows_alpha = windows_accent >> 24 & 0xff;
        int windows_red = windows_accent >> 16 & 0xff;
        int windows_green = windows_accent >> 8 & 0xff;
        int windows_blue = windows_accent & 0xff;

        if(5 * windows_green + 2 * windows_red + windows_blue >= 1024){
            this->setWindowIcon(QIcon(":/icons/viewer_black.ico"));
        }else{
            this->setWindowIcon(QIcon(":/icons/viewer_white.ico"));
        }

    }else{
        // linux etc for now ignored -> no windowicon
    }

}

/*
eventFilter
    :param obj: source
    :param event: event
*/
bool Application::eventFilter(QObject* obj, QEvent* event){
    switch(event->type()){
    case QEvent::MouseButtonRelease:{
        // The object name depends on the QMainWindow widget name
        // So if that is changed, the object name changes!
        // If not used multiple events will be send upon each click
        if(obj->objectName() == "Main::ControllerClassWindow"){
            emit this->globalMouseButtonRelease(event);
        }
        return false;
    }
    default:
        return false;
    }
}

/* 

#########################################################################
## The main window, system menu, and its layout
#########################################################################
class Application(QtWidgets.QApplication):


    def addSettings(self, settings):
        """ Adds Settings object to the application and sets global parameters """
        self.settings = settings
        self.setStyleSheet(self.settings.style.stylesheet)

    def reloadStyle(self):
        """ Reloads the QApplication stylesheet """
        self.setStyleSheet(self.settings.style.stylesheet)

class Window(QtWidgets.QMainWindow):
    """
    Custom QMainWindow constructs main window of the Fluorochrome Spectral Viewer and handles menu inputs
        :param parent: (optional) parent
    """
    def __init__(self, settings, parent=None):
        super(Window, self).__init__(parent)
        self.title = "Spectral Viewer"
        self.settings = settings
        self.standard_width = self.settings.defaults.value("DEFAULT/width", 800, type=int)
        self.standard_height = self.settings.defaults.value("DEFAULT/height", 300, type=int)

        # Sets MainWindow properties
        self.setGeometry(0, 0, self.standard_width, self.standard_height)
        self.setMinimumSize(self.standard_width, self.standard_height)

        self.setWindowTitle(self.title)

        self.installEventFilter(self)

        # Loads Data file from data.ini
        self.data = viewer_parsers.Data()

        # Menu menu-buttons
        self.menubar = QtWidgets.QMenuBar()
        self.menubar.setNativeMenuBar(True)
        self.file_menu = self.menubar.addMenu("&File")
        self.cytometer_menu = self.menubar.addMenu("&Cytometer")
        self.settings_menu = self.menubar.addMenu("&Settings")
        self.help_menu = self.menubar.addMenu("&Help")

        self.setMenuBar(self.menubar)

        # Fill the menu's
        self.file_menu = self.fileMenu(self.file_menu)
        self.cytometer_menu = self.cytometerMenu(self.cytometer_menu)
        self.settings_menu = self.settingsMenu(self.settings_menu)
        self.help_menu = self.helpMenu(self.help_menu)

        # Starts central widget
        self.central_widget = MainWidget(self.settings, self.data)
        self.setCentralWidget(self.central_widget)

        # Read menu settings
        self.readMenuSettings()
        self.central_widget.resetStyle()
        self.central_widget.resetMenu()

    # The menu's
    def fileMenu(self, menu):
        """
        Builds file menu QActions and returns those in a QMenu
            :param menu[QMenu]: menu to which QActions/QWidgets are added
            :returns: QMenu
        """
        menu_quit = QtWidgets.QAction("&Exit", self)
        menu_quit.setShortcut("Ctrl+Q")
        menu_quit.triggered.connect(self.fileMenuQuit)
        menu.addAction(menu_quit)

        return menu

    def cytometerMenu(self, menu):
        """
        Builds cytometer menu QActions and returns those in a QMenu
            :param menu[QMenu]: menu to which QActions/QWidgets are added
            :returns: QMenu
        """
        action_group = QtWidgets.QActionGroup(self)

        if self.data.cytometers is None:
            cytometer_action = QtWidgets.QAction("&None Loaded", self)
            cytometer_action.setCheckable(True)
            cytometer_action.setChecked(False)
            cytometer_action.setDisabled(True)
            action_group.addAction(cytometer_action)
        else:
            current_group = self.data.cytometers.group()
            if current_group != "":
                self.data.cytometers.endGroup()

            # Default None Selected
            cytometer_action = QtWidgets.QAction("&None", self)
            cytometer_action.setData("None")
            cytometer_action.triggered.connect(lambda boolean, cytometer="&None": self.changeCytometer(cytometer))
            cytometer_action.setCheckable(True)
            menu.addAction(cytometer_action)

            menu.addSeparator()

            # Add cytometers:
            for cytometer in self.data.cytometers.childGroups():
                self.data.cytometers.beginGroup(cytometer)
                name = self.data.cytometers.value("name", cytometer)
                cytometer_action = QtWidgets.QAction("&{}".format(name), self)
                cytometer_action.setData(cytometer)
                cytometer_action.triggered.connect(lambda boolean, cytometer=cytometer: self.changeCytometer(cytometer))
                cytometer_action.setCheckable(True)
                action_group.addAction(cytometer_action)

                self.data.cytometers.endGroup()

            if current_group != "":
                self.data.cytometers.beginGroup(current_group)

        menu.addActions(action_group.actions())
        return menu

    def settingsMenu(self, menu):
        """
        Builds settings menu QActions and returns those in a QMenu
            :param menu[QMenu]: menu to which QActions/QWidgets are added
            :returns: QMenu
        """
        self.sort_menu = menu.addMenu("Sorting")
        sort_options = ["Additive", "Alphabetical", "Excitation", "Emission"]
        sort_action_group = QtWidgets.QActionGroup(self)

        for option in sort_options:
            sort_action = QtWidgets.QAction("&{}".format(option), self)
            sort_action.triggered.connect(lambda boolean, name=option: self.changeSortOption(name))
            sort_action.setCheckable(True)
            sort_action_group.addAction(sort_action)
        self.sort_menu.addActions(sort_action_group.actions())

        self.sort_menu.addSeparator()

        sort_action = QtWidgets.QAction("&Reverse Sorting", self)
        sort_action.triggered.connect(lambda checked, name="reverse": self.changeSortOption(name, checked))
        sort_action.setCheckable(True)
        self.sort_menu.addAction(sort_action)

        sort_action = QtWidgets.QAction("&Pre-Sorting", self)
        sort_action.triggered.connect(lambda checked, name="presort": self.changeSortOption(name, checked))
        sort_action.setCheckable(True)
        self.sort_menu.addAction(sort_action)

        self.style_menu = menu.addMenu("Style")
        style_action_group = QtWidgets.QActionGroup(self)

        if self.settings.style is None:
            style_action = QtWidgets.QAction("&None Loaded", self)
            style_action.setCheckable(True)
            style_action.setChecked(False)
            style_action.setDisabled(True)
            style_action_group.addAction(style_action)
        else:
            current_group = self.settings.style.group()
            if current_group != "":
                self.settings.style.endGroup()

            # Add style:
            for style in self.settings.style.childGroups():
                self.settings.style.beginGroup(style)

                name = self.settings.style.value("name", style)
                style_action = QtWidgets.QAction("&{}".format(name), self)
                style_action.setData(style)
                style_action.triggered.connect(lambda boolean, style=style: self.changeStyle(style))
                style_action.setCheckable(True)
                style_action_group.addAction(style_action)

                self.settings.style.endGroup()

            if current_group != "":
                self.settings.style.beginGroup(current_group)
        self.style_menu.addActions(style_action_group.actions())

        menu.addSeparator()

        settings_action = QtWidgets.QAction("&Reset Settings", self)
        settings_action.triggered.connect(self.resetSettings)
        menu.addAction(settings_action)

        return menu

    def helpMenu(self, menu):
        """
        Builds help menu QActions and returns those in a QMenu
            :param menu[QMenu]: menu to which QActions/QWidgets are added
            :returns: QMenu
        """
        menu_license = QtWidgets.QAction("&License", self)
        license_widget = viewer_license.LicenseTabWidget()
        menu_license.triggered.connect(lambda: license_widget.show())
        menu.addAction(menu_license)

        return menu

    # Settings loading / saving
    def readSettings(self):
        """ restores the user's size and position settings """
        # Load parameters
        screen = self.settings.settings.value("USER/screen", None)
        screen_x = self.settings.settings.value("USER/screen_x", None)
        screen_y = self.settings.settings.value("USER/screen_y", None)
        screen_width = self.settings.settings.value("USER/screen_width", None)
        screen_height = self.settings.settings.value("USER/screen_height", None)
        pos_x = self.settings.settings.value("USER/pos_x", None)
        pos_y = self.settings.settings.value("USER/pos_y", None)
        width = self.settings.settings.value("USER/width", None)
        height = self.settings.settings.value("USER/height", None)
        fullscreen = self.settings.settings.value("USER/fullscreen", False, type=bool)

        # Check if screen is correct is not defaults to standard size and centrelized
        screen_reset = False
        screens = QtWidgets.QApplication.screens()
        if screen is None or screen_x is None or screen_y is None or screen_width is None or screen_height is None:
            screen_reset = True
        else:
            screen = int(screen)
            screen_x = int(screen_x)
            screen_y = int(screen_y)
            screen_width = int(screen_width)
            screen_height = int(screen_height)
            if screen >= len(screens):
                screen_reset = True
            else:
                screen_geom = screens[screen].availableGeometry()
                if screen_geom.x() != screen_x or screen_geom.y() != screen_y:
                    screen_reset = True
                elif screen_geom.width() != screen_width or screen_geom.height() != screen_height:
                    screen_reset = True

        if screen_reset is True:
            screen = QtWidgets.QApplication.desktop().primaryScreen()
            pos_x = None
            pos_y = None
            width = None
            height = None
            fullscreen = False

        screen = screens[screen]
        self.windowHandle().setScreen(screen)
        screen = screen.availableGeometry()

        # Load window width and height
        if width is None:
            width = self.standard_width
        else:
            width = int(width)
        if height is None:
            height = self.standard_height
        else:
            height = int(height)
        # load window x, y position
        if pos_x is None:
            pos_x = (screen.width() - self.standard_width) /2
        else:
            pos_x = int(pos_x)
        if pos_y is None:
            pos_y = (screen.height() - self.standard_height) /2
        else:
            pos_y = int(pos_y)

        self.resize(QtCore.QSize(width, height))
        self.move(QtCore.QPoint(pos_x, pos_y))

        if fullscreen is True:
            self.showFullScreen()

    def readMenuSettings(self):
        """ restores the menu's settings """
        # read cytometer
        if self.settings.settings is None:
            return

        cytometer_saved = self.settings.settings.value("USER/cytometer", self.settings.defaults.value("DEFAULT/cytometer", None))
        if cytometer_saved == "None":
            cytometer_saved = None

        for action in self.cytometer_menu.actions():
            if cytometer_saved is None:
                if action.data() == "None":
                    action.setChecked(True)
                else:
                    action.setChecked(False)
            else:
                if action.data() == cytometer_saved:
                    action.setChecked(True)
                else:
                    action.setChecked(False)

        self.data.changeCytometer(cytometer_saved)

        # read settings
        style_saved = self.settings.settings.value("USER/style", self.settings.defaults.value("DEFAULT/style", None))
        if style_saved == "None":
            style_saved = None

        sort_option_saved = self.settings.settings.value("USER/sort_option", self.settings.defaults.value("DEFAULT/sort_option", "Additive"))
        sort_reverse_saved = self.settings.settings.value("USER/sort_reversed", self.settings.defaults.value("DEFAULT/sort_reversed", False), type=bool)
        sort_presort_saved = self.settings.settings.value("USER/sort_presort", self.settings.defaults.value("DEFAULT/sort_presort", True), type=bool)

        for action in self.settings_menu.actions():
            if action.isSeparator() is True:
                continue
            elif action.menu() and action.text().lstrip("&") == "Sorting":
                for sub_action in action.menu().actions():
                    if sub_action.isSeparator() is True:
                        continue
                    else:
                        text = sub_action.text().lstrip("&")
                        if text in ["Additive", "Alphabetical", "Excitation", "Emission"]:
                            if text == sort_option_saved:
                                sub_action.setChecked(True)
                            else:
                                sub_action.setChecked(False)
                        elif text == "Reverse Sorting":
                            if sort_reverse_saved is True:
                                sub_action.setChecked(True)
                            else:
                                sub_action.setChecked(False)
                        elif text == "Pre-Sorting":
                            if sort_presort_saved is True:
                                sub_action.setChecked(True)
                            else:
                                sub_action.setChecked(False)
                            if sort_option_saved == "Emission" or sort_option_saved == "Excitation":
                                sub_action.setEnabled(True)
                            else:
                                sub_action.setEnabled(False)
            elif action.menu() and action.text().lstrip("&") == "Style":
                for sub_action in action.menu().actions():
                    if sub_action.data() == style_saved:
                        sub_action.setChecked(True)
                    else:
                        sub_action.setChecked(False)
            else:
                continue

        self.settings.changeStyle(style_saved)

    def writeSettings(self):
        """ write the user's size and position settings """
        screen = self.windowHandle().screen()
        screen_id = QtWidgets.QApplication.screens().index(screen)
        self.settings.settings.setValue("USER/screen", screen_id)
        self.settings.settings.setValue("USER/screen_x", screen.availableGeometry().x())
        self.settings.settings.setValue("USER/screen_y", screen.availableGeometry().y())
        self.settings.settings.setValue("USER/screen_width", screen.availableGeometry().width())
        self.settings.settings.setValue("USER/screen_height", screen.availableGeometry().height())
        self.settings.settings.setValue("USER/width", self.size().width())
        self.settings.settings.setValue("USER/height", self.size().height())
        self.settings.settings.setValue("USER/pos_x", self.pos().x())
        self.settings.settings.setValue("USER/pos_y", self.pos().y())
        self.settings.settings.setValue("USER/fullscreen", self.isFullScreen())

        # Save cytometer menu
        for action in self.cytometer_menu.actions():
            if action.isChecked() is True:
                self.settings.settings.setValue("USER/cytometer", action.data())
                break

        # Save (sorting) settings menu
        for action in self.settings_menu.actions():
            if action.isSeparator() is True:
                continue
            elif action.menu() and action.text().lstrip("&") == "Sorting":
                for sub_action in action.menu().actions():
                    if sub_action.isSeparator() is True:
                        continue
                    else:
                        text = sub_action.text().lstrip("&")
                        if text in ["Additive", "Alphabetical", "Excitation", "Emission"]:
                            if sub_action.isChecked() is True:
                                self.settings.settings.setValue("USER/sort_option", text)
                        elif text == "Reverse Sorting":
                            self.settings.settings.setValue("USER/sort_reversed", sub_action.isChecked())
                        elif text == "Pre-Sorting":
                            self.settings.settings.setValue("USER/sort_presort", sub_action.isChecked())
            elif action.menu() and action.text().lstrip("&") == "Style":
                for sub_action in action.menu().actions():
                    if sub_action.isSeparator() is True:
                        continue
                    else:
                        if sub_action.isChecked() is True:
                            self.settings.settings.setValue("USER/style", sub_action.data())
                            break
            else:
                continue

        self.settings.settings.sync()

    # Menu button functions
    def fileMenuQuit(self):
        """
        Close function of the file menu.
        """
        #self.central_widget.graph.writeSettings()
        self.writeSettings()

        QtWidgets.QApplication.quit()

    def changeCytometer(self, cytometer):
        """
        Changes the settings of the cytometer loaded into Settings and start resetting the GUI
            :param cytometer[str]: cytometer name
        """
        if cytometer == "&None":
            for action in self.cytometer_menu.actions():
                if action.text() != "&None":
                    action.setChecked(False)
            self.data.changeCytometer(None)
        else:
            for action in self.cytometer_menu.actions():
                if action.text() == "&None":
                    action.setChecked(False)
                    break
            self.data.changeCytometer(cytometer)
        self.central_widget.resetMenu()

    def changeSortOption(self, sort_option, checked=None):
        """
        Change sort option in menu and parses change towards MenuLayout
            :param sort_option[str]: sort_option as string
            :param checked[bool]: (optional) checked button state
        """
        if sort_option == "reverse" or sort_option == "presort":
            pass
        else:
            for action in self.sort_menu.actions():
                if action.text() == "&Pre-Sorting":
                    if sort_option == "Excitation" or sort_option == "Emission":
                        action.setEnabled(True)
                    else:
                        action.setEnabled(False)

        self.central_widget.changeSortOption(sort_option, setter=checked)

    def changeStyle(self, style):
        """
        Changes style in menu and forwards event to parser and QApplication
            :param style[str]: the style to change to
        """
        for action in self.style_menu.actions():
            if action.data() == style:
                action.setChecked(True)
            else:
                action.setChecked(False)

        self.settings.changeStyle(style)
        self.central_widget.resetStyle()

    def resetSettings(self):
        """ Reset the Settings settings and resets widget """
        self.settings.resetSettings()
        self.readSettings()
        self.readMenuSettings()
        self.central_widget.resetStyle()
        self.central_widget.resetMenu()

    # Event handling
    def resizeEvent(self, event):
        """
        Upon resizing / initialization this event is called.
        Hooks into resizeEvent to restore saved widget size upon first draw call
        """
        # This should limit the readSettings to only be run upon startup, not 100% sure though!
        if self.isVisible() is False:
            self.readSettings()
        super().resizeEvent(event)

    def eventFilter(self, source, event):
        """
        EventFilter for KeyPress events: MouseButtonPress.
            :param source: memory path of the events origin
            :param event[QEvent]: QEvent
        """
        if event.type() == QtCore.QEvent.MouseButtonPress:
            if self.underMouse() is True:
                self.central_widget.mainWindowMousePressEvent()
            return False
        if event.type() == QtCore.QEvent.Close:
            #self.central_widget.graph.writeSettings()
            self.writeSettings()
            return False
        return super(Window, self).eventFilter(source, event)

class MainWidget(QtWidgets.QWidget):
    """
    Main QWidget: populates the main QGridLayout and handles all universal events
        :param settings[Settings]: a Settings object
        :param data[Data]: a Data object
        :param parent: (optional) parent
    """
    def __init__(self, settings, data, parent=None):
        super(MainWidget, self).__init__()
        self.parent = parent
        self.settings = settings
        self.data = data

        # Builds layout
        layout = QtWidgets.QGridLayout()
        layout.setRowMinimumHeight(0, 1)
        layout.setRowStretch(0, 0)
        layout.setRowStretch(1, 1)
        layout.setColumnMinimumWidth(0, 80)
        layout.setColumnMinimumWidth(1, 80)
        layout.setColumnStretch(0, 0)
        layout.setColumnStretch(1, 1)
        layout.setContentsMargins(6, 6, 6, 6)
        layout.setSpacing(6)

        self.setLayout(layout)

        # Adds layout classes
        self.graph = viewer_graph_matplotlib.GraphScrollArea(self.data, self.settings, parent=self)
        self.laser = viewer_laser_menu.LaserLayout(self.graph, self.data, parent=self)
        self.menu = viewer_main_menu.MenuLayout(self.graph, self.data, self.settings, parent=self)
        self.barmenu = viewer_bar_menu.BarLayout(self.menu, self.graph, self.data, self.settings, parent=self)

        # Sets return reference and starts first graphPlot()
        self.graph.laser = self.laser
        self.graph.addGraphPlot()
        #self.graph.readSettings()

        self.layout().addLayout(self.laser, 0, 0, 1, 1, QtCore.Qt.AlignTop)
        self.layout().addLayout(self.barmenu, 0, 1, 1, 1, QtCore.Qt.AlignLeft)
        self.layout().addLayout(self.menu, 1, 0, 1, 1, QtCore.Qt.AlignTop)
        self.layout().addWidget(self.graph, 1, 1, 1, 1)

    def resetStyle(self):
        """ propagates resetStyle to graph, barmenu """
        self.barmenu.resetStyle()
        self.graph.resetStyle()

    def resetMenu(self):
        """ propagates resetSelf to graph, barmenu and laser """
        self.graph.resetPlot(visible_excitation=False, visible_emission=True, visible_detector=False)
        self.barmenu.resetSelf() # resetSelf reloads visibility values from graph
        self.laser.resetSelf()

    def changeSortOption(self, sort_option, setter=None):
        """
        Propagates changeSortOption to relevant GUI elements
            :param sort_option[str]: sort_option as string
            :param setter[bool]: (optional) setter for boolean values
        """
        self.menu.scroll_layout.changeSortOption(sort_option, setter=setter)

    def mainWindowMousePressEvent(self):
        """ Propagates mainWindowsMousePressEvent to menu, graph and laser"""
        self.menu.menu_line_edit.mainWindowMousePressEvent()
        self.graph.mainWindowMousePressEvent()
        self.laser.mainWindowMousePressEvent()


*/