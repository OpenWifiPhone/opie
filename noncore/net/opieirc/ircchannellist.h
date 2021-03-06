/*
    OpieIRC - An embedded IRC client
    Copyright (C) 2002 Wenzel Jakob

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __IRCCHANNELLIST_H
#define __IRCCHANNELLIST_H

#include <qlistbox.h>
#include "ircchannel.h"

class IRCChannelList : public QListBox {
public:
    IRCChannelList(IRCChannel *channel, QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    void update();
    void adjustNicks();
    bool hasPerson(QString nick);
    bool removePerson(QString nick);
protected:
    IRCChannel *m_channel;
};

#endif /* __IRCCHANNELLIST_H */
