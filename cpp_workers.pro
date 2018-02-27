TARGET = cpp_workers
CONFIG -= c++11
CONFIG -= qt
CONFIG -= app_bundle

QMAKE_CXXFLAGS = `xml2-config --cflags`
QMAKE_CXXFLAGS += -std=c++17 -Wall
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

LIBS = `xml2-config --libs`
LIBS += -lpthread
LIBS += -L$$_PRO_FILE_PWD_/3rd_party/static_libs/ -lgtest

QMAKE_LIBDIR = /usr/local/lib64

QMAKE_CXX = /usr/local/bin/g++
QMAKE_LINK = /usr/local/bin/g++

INCLUDEPATH += 3rd_party/include
INCLUDEPATH += src

SOURCES += \
    src/main/main.cpp \
    src/test/xml_worker_test.cpp

HEADERS += \
    3rd_party/include/gtest/gtest.h \
    src/common/enum_helper.h \
    src/common/enum_misc.h \
    src/common/enum_range.h \
    src/common/template_utils.h \
    src/work/workers.h \
    src/work/xml_worker.h \
    src/work/std_containers_impls.h \
    src/work/work.h \
    src/work/enum_helper.h \
    src/work/enum_misc.h \
    src/work/enum_range.h \
    src/work/template_utils.h \
    src/work/internal/enum_helper.h \
    src/work/internal/enum_misc.h \
    src/work/internal/enum_range.h \
    src/work/internal/template_utils.h
