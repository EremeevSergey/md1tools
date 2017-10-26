#include "printer_object.h"

CBasePrinterObject::CBasePrinterObject(CBasePrinterObject *parent)
{
    if (parent) __addChild (this);
    else ParentObject=0;
}

CBasePrinterObject::~CBasePrinterObject()
{
    // Удаляем себя из списка родителя
    if (ParentObject) ParentObject->__removeChild(this);
    // Удаляем детей
    for (int i=0,n=Children.size();i<n;i++){
        CBasePrinterObject* ch = Children.at(i);
        if (ch){
            ch->ParentObject = 0;
            delete ch;
        }
    }
    Children.clear();
}

void CBasePrinterObject::__addChild   (CBasePrinterObject* child)
{
    child->ParentObject = this;
    Children.append(child);
}

void CBasePrinterObject::__removeChild(CBasePrinterObject* child)
{
    for (int i=0,n=Children.size();i<n;i++){
        CBasePrinterObject* ch = Children.at(i);
        if (ch && ch==child){
            ch->ParentObject = 0;
            delete ch;
            Children.removeAt(i);
            break;
        }
    }
}

bool CBasePrinterObject::__sendCommand(const QString&   cmd_string,
                                     EPrinterCommands cmd_type,
                                     bool wait)
{
    if (ParentObject)
        return ParentObject->__sendCommand (cmd_string,cmd_type,wait);
    return false;
}

bool CBasePrinterObject::__sendCommands (const CPrinterScript& cmd_string,
                                       EPrinterCommands   cmd_type)
{
    if (ParentObject)
        return ParentObject->__sendCommands (cmd_string,cmd_type);
    return false;
}

void CBasePrinterObject::setErrorString (const QString& str, CBasePrinterObject *sender)
{
    if (ParentObject)
        ParentObject->setErrorString (str,sender);
}

bool CBasePrinterObject::parsePrinterAnswer(const QString& input,EPrinterCommands cmd_type)
{
    for (int i=0,n=Children.size();i<n;i++){
        CBasePrinterObject* ch = Children.at(i);
        if (ch && ch->parsePrinterAnswer(input,cmd_type)) return true;
    }
    return false;
}

