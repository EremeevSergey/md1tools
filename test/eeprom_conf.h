#ifndef EEPROM_CONF_H
#define EEPROM_CONF_H
#include <QAbstractTableModel>
#include <QItemDelegate>
#include "../gui/basewnd.h"
#include "ui_eeprom_conf.h"
#include "../common.h"
#include "../core/printer/commands.h"

class QEepromDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    QEepromDelegate(QObject* parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
class QEepromTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    QEepromTableModel(QObject * parent = 0);
    void              clear();
    void              append(CEePromRecord* rec);
    CEePromRecord*    at    (int i);
    int               count(){return Values.count();}
    void              update();
    int               rowCount(const QModelIndex & parent = QModelIndex()) const;
    int               columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant          data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant          headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags     flags(const QModelIndex & index) const;
    QModelIndex       index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    bool              setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
protected:
    CEePromList       Values;
};

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/

class CEepromConfiguration : public CBaseWindow,public Ui::eeprom_conf
{
    Q_OBJECT

public:
    explicit CEepromConfiguration(QWidget *parent = 0);
    ~CEepromConfiguration();
protected:
    bool updateModel;
protected slots:
//    void slotReady(const QString& name);
    void slotCommandReady (int cmd_type);
private slots:
    void on_pbRefresh_clicked();

    void on_pbRead_clicked();

    void on_pbWrite_clicked();

    void on_pbSave_clicked();

    void on_pbLoad_clicked();

private:
    QEepromTableModel* Model;
};

#endif // EEPROM_CONF_H
