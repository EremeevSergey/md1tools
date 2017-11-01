#include "commands.h"
#include <QDebug>

CPrinterScript::CPrinterScript()
{

}

CPrinterScript::CPrinterScript(const QStringList& list)
{
    for (int i=0,n=list.size();i<n;i++){
        Items.append(CPrinterScriptCommand(list.at(i),true));
    }
}



//FIRMWARE_NAME:Repetier_0.92.9 FIRMWARE_URL:https://github.com/repetier/Repetier-Firmware/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:Delta EXTRUDER_COUNT:1 REPETIER_PROTOCOL:3
//Printed filament:20.29m Printing time:0 days 7 hours 12 min
//PrinterMode:FFF

//FIRMWARE_NAME:Repetier_0.92.9 FIRMWARE_URL:https://github.com/repetier/Repetier-Firmware/ PROTOCOL_VERSION:1.0 MACHINE_TYPE:Delta EXTRUDER_COUNT:1 REPETIER_PROTOCOL:3
//Printed filament:20.29m Printing time:0 days 7 hours 12 min
//PrinterMode:FFF

CPrinterCapabilities::CPrinterCapabilities()
{

}

bool CPrinterCapabilities::isPresent  (const QString& name) const
{
    return Items.contains(name);
}

QString CPrinterCapabilities::value (const QString& name)
{
    if (Items.contains(name)) return Items[name];
    return QString();
}

//QString parseValue(QString& name)
//{
//    QString
//}

int CPrinterCapabilities::parseString(const QString& name)
{
    QStringList list = name.split(" ");
    if (list.size()>1){
        for (int i=0,n=list.size();i<n;i++){
            QString str = list.at(i);
//            qDebug() << str;
            int pos = str.indexOf(":");
            int len = str.length();
            if (pos>0 && pos<len){
                Items[str.left(pos)]=str.right(len-pos-1);
            }
            else{
                Items.clear();
                return 0;
            }
        }
    return Items.size();
    }
    return 0;
}

