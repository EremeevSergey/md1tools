#ifndef PRINTER_COMMANDS_H
#define PRINTER_COMMANDS_H
#include <QStringList>

enum EPrinterCommands{
    EPrinterCommandsNone=0,
    EPrinterCommandsM205,
    EPrinterCommandsM206
};

struct CPrinterScriptCommand{
    QString  Command;
    bool     Wait;

    CPrinterScriptCommand(){Wait=true;}
    CPrinterScriptCommand(const QString& cmd)       {Command=cmd; Wait=true;}
    CPrinterScriptCommand(const QString& cmd,bool w){Command=cmd; Wait=w;}
    CPrinterScriptCommand& operator=(const CPrinterScriptCommand& right) {
            //проверка на самоприсваивание
            if (this == &right) return *this;
            Command = right.Command;
            Wait    = right.Wait;
            return *this;
        }
};

class CPrinterScript
{
public:
    CPrinterScript();
    CPrinterScript(const QStringList& list);

    inline void append (const QString& str)                {Items.append(CPrinterScriptCommand(str,true));}
    inline void append (const QString& str,bool w)         {Items.append(CPrinterScriptCommand(str,w   ));}
    inline void append (const CPrinterScriptCommand& item) {Items.append(item);}
    inline int  size () const {return Items.size();}
    inline int  count() const {return Items.count();}
    inline void removeFirst() {Items.removeFirst();}
    inline const CPrinterScriptCommand&  at(int i) const
                                    {return Items.at(i);}
    inline const CPrinterScriptCommand&  first() const
                                    {return Items.first();}
    CPrinterScript& operator=(const CPrinterScript& right) {
            //проверка на самоприсваивание
            if (this == &right) return *this;
            Items = right.Items;
            return *this;
        }
protected:
    QList<CPrinterScriptCommand> Items;
};



#endif // PRINTER_COMMANDS_H
