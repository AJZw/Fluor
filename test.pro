TEMPLATE += app
CONFIG += debug console c++14
QT += widgets

HEADERS += tests/catch.hpp
SOURCES += tests/test_main.cpp
SOURCES += tests/test_file_settings.cpp tests/test_file_fluorophores.cpp tests/test_file_styles.cpp
SOURCES += tests/test_data_styles.cpp

HEADER += src/viewer_data_factory.h src/viewer_data_fluorophores.h src/viewer_data_style.h
SOURCES += src/viewer_data_factory.cpp src/viewer_data_fluorophores.cpp src/viewer_data_style.cpp

INCLUDEPATH += $$PWD/src $$PWD/tests 
INCLUDEPATH += C:/Qt/5.11.2/mingw53_32/include/QtCore/5.11.2 C:/Qt/5.11.2/mingw53_32/include/QtGui/5.11.2

CONFIG(debug, debug|release) {
    DESTDIR = tests/debug
} else {
    DESTDIR = tests/release
}

OBJECTS_DIR = $${DESTDIR}/.generatedfiles
MOC_DIR = $${DESTDIR}/.generatedfiles
RCC_DIR = $${DESTDIR}/.generatedfiles
UI_DIR = $${DESTDIR}/.generatedfiles
