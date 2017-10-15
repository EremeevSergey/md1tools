#ifndef CONFIG_H
#define CONFIG_H
#include <QSettings>

class CConfig
{
public:
    CConfig ();
    ~CConfig();
    QString         PortName;
    qint32          BaudRate;
    int             WindowsHeight;
    int             WindowsWidth;
    void save();
    bool load();
protected:
    QSettings* Settings;

};
#endif //CONFIG_H
