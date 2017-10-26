#include "commands.h"

CPrinterScript::CPrinterScript()
{

}

CPrinterScript::CPrinterScript(const QStringList& list)
{
    for (int i=0,n=list.size();i<n;i++){
        Items.append(CPrinterScriptCommand(list.at(i),true));
    }
}

