/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef DATEBOOKDAYHEADER_H
#define DATEBOOKDAYHEADER_H
#include <qdatetime.h>
#include "datebookweekheader.h"


class DateBookWeekHeader : public DateBookWeekHeaderBase
{
    Q_OBJECT

public:
    DateBookWeekHeader( bool startOnMonday, QWidget* parent = 0,
			const char* name = 0, WFlags fl = 0 );
    ~DateBookWeekHeader();

    void setDate( int y, int w );
    void setStartOfWeek( bool onMonday );

signals:
    void dateChanged( int y, int w );

public slots:
	void pickDate();
	void yearChanged( int );
	void nextMonth();
	void prevMonth();
	void nextWeek();
	void prevWeek();
	void weekChanged( int );
	void setDate( int y, int m, int d);

protected slots:
    void keyPressEvent(QKeyEvent *e)
    {
	e->ignore();
    }

private:
    int year,
	week;
    bool bStartOnMonday;

};

QDate dateFromWeek( int week, int year, bool startOnMonday );

#endif // DATEBOOKDAYHEADER_H
