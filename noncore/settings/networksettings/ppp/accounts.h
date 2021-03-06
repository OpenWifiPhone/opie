/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: accounts.h,v 1.5 2003-08-09 17:14:55 kergoth Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _ACCOUNTS_H_
#define _ACCOUNTS_H_

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include "chooserwidget.h"

class QCheckBox;
class QLineEdit;
class QTabWidget;
class ModemWidget;
class ModemWidget2;


class DialWidget;
class AuthWidget;
class IPWidget;
class DNSWidget;
class GatewayWidget;
class PPPData;


class AccountWidget : public ChooserWidget {

  Q_OBJECT
public:
  AccountWidget( PPPData *pd, QWidget *parent=0, const char *name=0, WFlags f=0 );
  ~AccountWidget() {}


private slots:
  virtual void edit();
  virtual void copy();
  virtual void remove();
  virtual void create();
  virtual void slotListBoxSelect(int);
  int doTab();

 signals:
   void resetaccounts();

private:
    DialWidget *dial_w;
    IPWidget *ip_w;
    DNSWidget *dns_w;
    GatewayWidget *gateway_w;
    AuthWidget *auth_w;

};


#endif

