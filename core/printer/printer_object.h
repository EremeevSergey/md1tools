#ifndef PRINTER_OBJECT_H
#define PRINTER_OBJECT_H
#include <QList>
#include <QStringList>
#include <QObject>
#include "commands.h"

class CBasePrinterObject
{
public:
    CBasePrinterObject(CBasePrinterObject* parent);
    virtual           ~CBasePrinterObject();
    virtual bool       parsePrinterAnswer(const QString& input,EPrinterCommands cmd_type);
    QString            getName           (){return Name;}
    virtual void       setErrorString    (const QString& str,CBasePrinterObject *sender);
    virtual bool       __sendCommand     (const QString&     cmd_string,
                                          EPrinterCommands   cmd_type,
                                          bool wait);
    virtual bool       __sendCommands    (const CPrinterScript& cmd_string,
                                          EPrinterCommands cmd_type);
protected:
    CBasePrinterObject* ParentObject;
    QString             Name;
private:
    QList<CBasePrinterObject*> Children;
    void                __addChild   (CBasePrinterObject* child);
    void                __removeChild(CBasePrinterObject* child);
};

#endif // PRINTER_OBJECT_H
