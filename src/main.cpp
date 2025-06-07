/***************************************************************************
 *   Copyright (C) 2008 by Oliver Becker   *
 *   der.ole.becker@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>

#include "mainwindow.h"

int main ( int argc, char *argv[] )
{
	// Create application
	QApplication app ( argc, argv );
	
	app.setApplicationName("QVideoConverter");
// 	app.setApplicationVersion("0.1");
	
	QTranslator translator;
	translator.load(QString(":/languages/lang_%1").arg(QLocale::system().name()));
	app.installTranslator(&translator);
	
	QTranslator qttranslator;
	qttranslator.load(QString(":/languages/qt_%1").arg(QLocale::system().name()));
	app.installTranslator(&qttranslator);
	
	// Create main window and show it
	MainWindow mw;
	mw.show();
	
	// Execute application
	return app.exec();
}

