#include <QStringList>
#include <QDebug>
#include "eeprom.h"

/*****************************************************************************\
*                                  CEeProm                                    *
*                                Класс EEPROM                                 *
\*****************************************************************************/
CEeProm::CEeProm(CBasePrinterObject *parent):
    CBasePrinterObject(parent)
{
    Name = "EEPROM";
}

CEeProm::CEeProm(const CEePromList& other, CBasePrinterObject *parent) :
    CBasePrinterObject(parent),CEePromList(other)
{
}

bool CEeProm::read  ()
{
    clear();
    if (ParentObject)
        return ParentObject->__sendCommand("M205",EPrinterCommandsM205,true);
    return false;
}

bool CEeProm::parsePrinterAnswer(const QString& input,EPrinterCommands cmd_type)
{
//    qDebug() << input;
    if (cmd_type==EPrinterCommandsM205){
        CEePromRecord* r = new CEePromRecord();
        if (r->FromString(input)) {
            ValueList.append(r);
//            qDebug() << r->ToString();
            return true;
        }
        else
            delete r;
    }
    return false;
}

bool CEeProm::write ()
{
    if (ParentObject){
        CPrinterScript script;
        for (int i=0,n=ValueList.count();i<n;i++){
            script.append(ValueList.at(i)->ToCmdString(),false);
        }
        if (ParentObject)
            return ParentObject->__sendCommands(script,EPrinterCommandsM206);
    }
    return false;
}

bool CEeProm::writeParameter(int index)
{
    if (ParentObject && index>=0 && index<ValueList.size()){
        CEePromRecord* r = ValueList.at(index);
        if (r && ParentObject)
            return ParentObject->__sendCommand(r->ToCmdString(),EPrinterCommandsM206,false);
    }
    return false;
}

bool CEeProm::writeParameter(const QString& name)
{
    for (int i=0,n=ValueList.size();i<n;i++){
        CEePromRecord* r = ValueList.at(i);
        if (r && r->Description==name && ParentObject)
            return ParentObject->__sendCommand(r->ToCmdString(),EPrinterCommandsM206,false);
    }
    return false;
}

/*****************************************************************************\
*                               CEePromRecord                                 *
*                         Класс одной записи EEPROM                           *
\*****************************************************************************/
CEePromRecord::CEePromRecord()
{
    Type = Unknown;
    FValue = 0;
    IValue = 0;
    Position = 0;
}

CEePromRecord::CEePromRecord(const CEePromRecord& v)
{
    Type        = v.Type;
    Position    = v.Position;
    FValue      = v.FValue;
    IValue      = v.IValue;
    Description = v.Description;
}

CEePromRecord::CEePromRecord(CEePromRecord* v)
{
    if (v){
        Type        = v->Type;
        Position    = v->Position;
        FValue      = v->FValue;
        IValue      = v->IValue;
        Description = v->Description;
    }
    else{
        Type = Unknown;
        FValue = 0;
        IValue = 0;
        Position = 0;
    }
}

bool CEePromRecord::isEqual(const CEePromRecord* v) const
{
    if (v && Type == v->Type){
        if (Type==Float){
            if (FValue == v->FValue) return true;
        }
        else {
            if (IValue == v->IValue) return true;
        }
    }
    return false;
}

char strEPR[]="EPR:";

QString CEePromRecord::ToString   () const
{
    //EPR:2 75 250000 Baudrate
    QString ret;
    if (Type!=Unknown){
        QString val;
        if (Type==Float) val = QString("%1").arg(QString::number(FValue,'f',3));
        else             val = QString("%1").arg(QString::number(IValue));
        ret = QString("EPR:%1 %2 %3 %4").arg(QString::number(Type),
                                             QString::number(Position),
                                             val,
                                             Description);
    }
    return ret;
}

QString CEePromRecord::ToCmdString(CEePromRecord* ptr)
{
    QString ret;
    if (ptr->Type!=Unknown){
        QString val;
        if (ptr->Type==Float) val = QString("X%1").arg(QString::number(ptr->FValue,'f',3));
        else                  val = QString("S%1").arg(QString::number(ptr->IValue));
        ret = QString("M206 T%1 P%2 %3").arg(QString::number(ptr->Type),
                                             QString::number(ptr->Position),
                                             val);
    }
    return ret;
}

bool CEePromRecord::isValid() const
{
    if (Type!=Unknown) return true;
    return false;
}

bool CEePromRecord::FromString(const QString& str)
{
    QString in = str.trimmed();
    //EPR:2 75 250000 Baudrate
    Type = Unknown;
    if (in.toUpper().startsWith(strEPR)){
        in = in.right(str.length()-strlen(strEPR));
        // The first value is the data_type (0=byte, 1=16 bit int,2=32 bit int,3 = float).
        bool ok;
        int pos=in.indexOf(' ',0);
        if (pos>=0){
            int t = in.left(pos).toInt(&ok);
            if (ok && t>=0 && t<=3){
            //  The second parameter is the position in EEPROM, the value is stored.
                in = in.right(in.length()-pos).trimmed();
                pos=in.indexOf(' ',0);
                if (pos>=0){
                    Position = in.left(pos).toInt(&ok);
                    if (ok){
                        in = in.right(in.length()-pos).trimmed();
                        //  The third parameter is the current value.
                        pos=in.indexOf(' ',0);
                        if (pos>=0){
                            if (t == 3) FValue = in.left(pos).toFloat(&ok);
                            else        IValue = in.left(pos).toLong(&ok);
                            if (ok){
                                Type = t;
                                Description = in.right(in.length()-pos).trimmed();
                                return true;
                            }
                        }
                    }
                }
            }
        }

    }
    return false;
}




/*****************************************************************************\
*                                CEePromList                                  *
*                        Класс списка значений EEPROM                         *
\*****************************************************************************/

CEePromList::CEePromList()
{
}

CEePromList::CEePromList(const CEePromList& other)
{
    for (int i=0,n=other.count();i<n;i++){
        CEePromRecord* rec = new CEePromRecord(*(other.ValueList.at(i)));
        ValueList.append(rec);
    }
}

CEePromList::~CEePromList()
{
    clear();
}

void CEePromList::clear()
{
    for (int i=0,n=ValueList.size();i<n;i++){
        CEePromRecord* val= ValueList.at(i);
        if (val) delete val;
    }
    ValueList.clear();
}

CEePromRecord *CEePromList::at(int i) const
{
    if (i>=0 && i<ValueList.size()) return ValueList.at(i);
    return 0;
}

CEePromRecord* CEePromList::atByStartName(const QString &name) const
{
    if (name.length()>4)
        for (int i=0,n=ValueList.size();i<n;i++){
            CEePromRecord* val= ValueList.at(i);
            if (val && val->Type!=CEePromRecord::Unknown && val->Description.startsWith(name)) return val;
        }
    return 0;
}

void CEePromList::append (const CEePromRecord* r)
{
    if (r!=0){
        CEePromRecord* a = new CEePromRecord(*r);
        ValueList.append(a);
    }
}

int CEePromList::indexByName(const QString& name) const
{
    for (int i=0,n=ValueList.size();i<n;i++){
        CEePromRecord* val= ValueList.at(i);
        if (val && val->Type!=CEePromRecord::Unknown && val->Description == name) return i;
    }
    return -1;
}

int CEePromList::indexByStartName(const QString &name) const
{
    if (name.length()>4)
        for (int i=0,n=ValueList.size();i<n;i++){
            CEePromRecord* val= ValueList.at(i);
            if (val && val->Type!=CEePromRecord::Unknown && val->Description.startsWith(name)) return i;
        }
    return -1;
}

bool CEePromList::setParameterValue(int index,qint32 val)
{
    if (index>=0 && index<ValueList.size()){
        CEePromRecord* r = ValueList.at(index);
        if (r){
            switch (r->Type) {
            case CEePromRecord::Byte:
                if (val>255) val = 255;
                r->IValue = val;
                return true;
            case CEePromRecord::Int16:
                if (val> 32767) val =  32767;
                if (val<-32768) val = -32768;
                r->IValue = val;
                return true;
            case CEePromRecord::Int32:
                r->IValue = val;
                return true;
            default:
                break;
            }
        }
    }
    return false;
}

bool CEePromList::setParameterValue(int index,float val)
{
    if (index>=0 && index<ValueList.size()){
        CEePromRecord* r = ValueList.at(index);
        if (r && r->Type==CEePromRecord::Float){
            r->FValue = val;
            return true;
        }
    }
    return false;
}


