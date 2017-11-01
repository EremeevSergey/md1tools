#include "config.h"
#include <QStringList>

char strWindowsHeight   []="WindowsHeight";
char strWindowsWidth    []="WindowsWidth";
char strBaudRate        []="BaudRate";
char strPortName        []="PortName";
char strOrganizationName[]="Micromake";
char strApplicationName []="MD1Config";

CConfig::CConfig ()
{
    Settings = new QSettings(strOrganizationName,strApplicationName);
    BaudRate = 115200;
    WindowsHeight = 100;
    WindowsWidth  = 200;
}

CConfig::~CConfig()
{
    save();
    delete Settings;
}

void CConfig::save()
{
    Settings->setValue(strWindowsHeight,WindowsHeight);
    Settings->setValue(strWindowsWidth ,WindowsWidth);
    Settings->setValue(strPortName,PortName);
    Settings->setValue(strBaudRate,BaudRate);
}

bool CConfig::load()
{
    PortName = Settings->value(strPortName).toString();
    bool b=false;
    BaudRate = Settings->value(strBaudRate).toInt(&b);
    if (b!=true) BaudRate = 115200;
    WindowsHeight = Settings->value(strWindowsHeight).toInt(&b);
    if (b!=true) WindowsHeight = 480;
    WindowsWidth = Settings->value(strWindowsWidth).toInt(&b);
    if (b!=true) WindowsWidth = 640;
    return true;
}
