/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "application.h"

#include <QDebug>
#include <QIcon>
#include <QSettings>
#include <QSysInfo>
#include <QWidget>

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {
  this->setOrganizationName("AJZw");
  this->setApplicationName("2023-01-04");  // used as release data storage
  this->setApplicationVersion("0.10.2");

  this->installEventFilter(this);

  // Set icon
  if (QSysInfo().productType() == "osx") {
    this->setWindowIcon(QIcon(":/icons/fluor_light.icns"));
  } else if (QSysInfo().productType() == "windows") {
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\DWM", QSettings::NativeFormat);
    unsigned int windows_accent = registry.value("AccentColor", 0xffffffff).toUInt();
    // int windows_alpha = windows_accent >> 24 & 0xff;
    int windows_red = windows_accent >> 16 & 0xff;
    int windows_green = windows_accent >> 8 & 0xff;
    int windows_blue = windows_accent & 0xff;

    if (5 * windows_green + 2 * windows_red + windows_blue >= 1024) {
      this->setWindowIcon(QIcon(":/icons/fluor_light.ico"));
    } else {
      this->setWindowIcon(QIcon(":/icons/fluor_dark.ico"));
    }

  } else {
    this->setWindowIcon(QIcon(":/icons/fluor_light.icns"));
  }
}

/*
eventFilter
  :param obj: source
  :param event: event
*/
bool Application::eventFilter(QObject* obj, QEvent* event) {
  switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease: {
      // The object name depends on the QMainWindow widget name
      // So if that is changed, the object name changes!
      // If not used multiple events will be send upon each click
      if (obj->objectName() == "Main::ControllerClassWindow") {
        emit this->globalEvent(event);
      }
      return false;
    }
    default: {
      return false;
    }
  }
}
