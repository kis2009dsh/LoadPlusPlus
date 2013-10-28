#############################################################################
# Makefile for building: PresureTest.app/Contents/MacOS/PresureTest
# Generated by qmake (3.0) (Qt 5.1.1) on: ?? 9? 16 11:08:48 2013
# Project:  PresureTest.pro
# Template: app
# Command: /Users/dongshihao/Qt5.1.1/5.1.1/clang_64/bin/qmake -spec macx-xcode -o PresureTest.xcodeproj/project.pbxproj PresureTest.pro
#############################################################################

MAKEFILE      = project.pbxproj

MOC       = /Users/dongshihao/Qt5.1.1/5.1.1/clang_64/bin/moc
UIC       = /Users/dongshihao/Qt5.1.1/5.1.1/clang_64/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH       = -I../Qt5.1.1/5.1.1/clang_64/mkspecs/macx-xcode -I. -I../Qt5.1.1/5.1.1/clang_64/include -I../Qt5.1.1/5.1.1/clang_64/include/QtWidgets -I../Qt5.1.1/5.1.1/clang_64/lib/QtWidgets.framework/Versions/5/Headers -I../Qt5.1.1/5.1.1/clang_64/include/QtGui -I../Qt5.1.1/5.1.1/clang_64/lib/QtGui.framework/Versions/5/Headers -I../Qt5.1.1/5.1.1/clang_64/include/QtCore -I../Qt5.1.1/5.1.1/clang_64/lib/QtCore.framework/Versions/5/Headers -I. -I/Users/dongshihao/Downloads/xcode/Xcode -I4.5.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/System/Library/Frameworks/OpenGL.framework/Headers -I/Users/dongshihao/Downloads/xcode/Xcode -I4.5.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/System/Library/Frameworks/AGL.framework/Headers/ -I. -F/Users/dongshihao/Qt5.1.1/5.1.1/clang_64/lib
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

compilers: ./moc_mainwindow.cpp ./ui_mainwindow.h
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_header_make_all: moc_mainwindow.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainwindow.cpp
moc_mainwindow.cpp: ../Qt5.1.1/5.1.1/clang_64/include/QtWidgets/QMainWindow \
		mainwindow.h
	/Users/dongshihao/Qt5.1.1/5.1.1/clang_64/bin/moc $(DEFINES) -D__APPLE__ -D__GNUC__=4 $(INCPATH) mainwindow.h -o moc_mainwindow.cpp

compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_mainwindow.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h
ui_mainwindow.h: mainwindow.ui
	/Users/dongshihao/Qt5.1.1/5.1.1/clang_64/bin/uic mainwindow.ui -o ui_mainwindow.h

compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 
