
/*
 * debug value mapper - generated by regen.py - (C) Michael 'Mickey' Lauer <mickey@vanille.de>
 */

#ifndef DEBUGMAPPER_H
#define DEBUGMAPPER_H

#include <qstring.h>
#include <qintdict.h>

typedef QIntDict<QString> IntStringMap;

class DebugMapper
{
  public:
    DebugMapper();
    ~DebugMapper();

    const QString& map( int value ) const;
  private:
    IntStringMap _map;
};

#endif

