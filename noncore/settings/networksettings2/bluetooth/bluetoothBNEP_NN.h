#ifndef BLUETOOTHBNEP_NETNODE_H
#define BLUETOOTHBNEP_NETNODE_H

#include "netnode.h"
#include "bluetoothRFCOMM_NN.h"

class ABluetoothBNEP;

class BluetoothBNEPNetNode : public ANetNode {

      Q_OBJECT

public:

      BluetoothBNEPNetNode();
      virtual ~BluetoothBNEPNetNode();

      virtual QString genNic( long );
      virtual long instanceCount( void )
        { return InstanceCount; }
      virtual const QString pixmapName()
        { return "Devices/bluetooth"; }

      virtual const QString nodeDescription() ;
      virtual ANetNodeInstance * createInstance( void );
      virtual const char ** provides( void );
      virtual const char ** needs( void );

private:

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

      // number of interfaces for this device
      long InstanceCount;

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif
