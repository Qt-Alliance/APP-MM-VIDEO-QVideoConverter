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
 
#include "tools.h"

uint regCaps(const QString & reg, const QString & line, QString * cap1, QString * cap2, QString * cap3, QString * cap4, QString * cap5, QString * cap6, QString * cap7)
{
	// Regular expression
	QRegExp exp(reg);
	// Check if we have an exact match
	if(!exp.exactMatch(line))
		return 0;
	
	// Get captured texts
	QStringList list = exp.capturedTexts();
	// Check if list has items
	if(list.count())
		// Remove first item (the full line of captured texts)
		list.removeFirst();
	
	// Check if list has enough items and the QString pointer is not NULL
	if(list.count() > 0 && cap1)
		// Assign captured text
		*cap1 = list[0];
	if(list.count() > 1 && cap2)
		*cap2 = list[1];
	if(list.count() > 2 && cap3)
		*cap3 = list[2];
	if(list.count() > 3 && cap4)
		*cap4 = list[3];
	if(list.count() > 4 && cap5)
		*cap5 = list[4];
	if(list.count() > 5 && cap6)
		*cap6 = list[5];
	if(list.count() > 6 && cap7)
		*cap7 = list[6];
	
	// Return number of captured texts, but maximum 5
	return qMin(list.count(),5);
}
