TEMPLATE = subdirs

QT += core widgets gui testlib
CONFIG += c++17 object_parallel_to_source
DEFINES += "PROJECT_ASSETS=\\\"$$PWD/assets\\\""

TEMPLATE = subdirs
SUBDIRS = $$PWD/Newpson/Parsing/Obj/ParserDemo
          $$PWD/Newpson/Parsing/Obj/ParserRunUnitTests/RunUnitTests

SUBDIRS = \
    $$PWD/Newpson/Parsing/Obj/ParserDemo \
    $$PWD/Newpson/Parsing/Obj/ParserRunUnitTests
# include($$PWD/Newpson/Parsing/Obj/ParserDemo/Demo.pro)
# include($$PWD/Newpson/Parsing/Obj/ParserRunUnitTests/RunUnitTests.pro)
