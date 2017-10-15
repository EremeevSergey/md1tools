#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include "eeprom_conf.h"
#include "../core/core.h"
#include "../common.h"

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
QEepromDelegate::QEepromDelegate(QObject* parent):QItemDelegate(parent)
{

}


QWidget* QEepromDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    if (index.isValid()){
        CEePromRecord* rec = (CEePromRecord*)(index.internalPointer());
        if (rec){
            if (rec->Type == CEePromRecord::Float){
                QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
//                editor->setFrame(false);
                editor->setDecimals(3);
                editor->setMinimum(-10000000);
                editor->setMaximum(10000000);
                return editor;
            }
            else if (rec->Type == CEePromRecord::Byte){
                QSpinBox* editor = new QSpinBox(parent);
//                editor->setFrame(false);
                editor->setMinimum(0);
                editor->setMaximum(255);
                return editor;
            }
            else if (rec->Type == CEePromRecord::Int16){
                QSpinBox* editor = new QSpinBox(parent);
//                editor->setFrame(false);
                editor->setMinimum(-32768);
                editor->setMaximum(32767);
                return editor;
            }
            else if (rec->Type == CEePromRecord::Int32){
                QSpinBox* editor = new QSpinBox(parent);
//                editor->setFrame(false);
                editor->setMinimum(-10000000);
                editor->setMaximum(10000000);
                return editor;
            }
        }
    }
    return QItemDelegate::createEditor(parent,option,index);
}

void QEepromDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.isValid()){
        CEePromRecord* rec = (CEePromRecord*)(index.internalPointer());
        if (rec->Type == CEePromRecord::Float){
            float value = index.model()->data(index,Qt::EditRole).toFloat();
            QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
            spinBox->setValue(value);
        }
        else if (rec->Type == CEePromRecord::Byte  ||
                 rec->Type == CEePromRecord::Int16 ||
                 rec->Type == CEePromRecord::Int32){
            float value = index.model()->data(index,Qt::EditRole).toInt();
            QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
            spinBox->setValue(value);
        }
    }
}

void QEepromDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    if (index.isValid()){
        CEePromRecord* rec = (CEePromRecord*)(index.internalPointer());
        if (rec->Type == CEePromRecord::Float){
            QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
            spinBox->interpretText();
            model->setData(index, spinBox->value(), Qt::EditRole);
        }
        else if (rec->Type == CEePromRecord::Byte  ||
                 rec->Type == CEePromRecord::Int16 ||
                 rec->Type == CEePromRecord::Int32){
            QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
            spinBox->interpretText();
            model->setData(index, spinBox->value(), Qt::EditRole);
        }
    }
}

void QEepromDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
QEepromTableModel::QEepromTableModel(QObject * parent):QAbstractTableModel(parent)
{
}

void QEepromTableModel::clear()
{
    beginResetModel();
    Values.clear();
    endResetModel();
}

void QEepromTableModel::append(CEePromRecord* rec)
{
    if (rec){
        beginResetModel();
        Values.append(rec);
        endResetModel();
    }
}

CEePromRecord* QEepromTableModel::at (int i)
{
     return Values.at(i);
}

QModelIndex QEepromTableModel::index(int row, int column,const QModelIndex &parent) const
{
    Q_UNUSED (parent);
    if (column == 0 && row>=0 && row<Values.count()){
        return createIndex(row,column,Values.at(row));
    }
    return QModelIndex();
}

int QEepromTableModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return Values.count();
}

int QEepromTableModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant QEepromTableModel::data(const QModelIndex & index, int role) const
{
    if (index.isValid()){
        int      col = index.column();
        int      row = index.row();
        if (row>=0 && row<Values.count()){
            const CEePromRecord* rec1;
            const CEePromRecord* rec2;
            if (role==Qt::DisplayRole || role ==Qt::EditRole){
                rec1 = Values.at(row);
                if (rec1 && col==0){
                    if (rec1->Type==CEePromRecord::Float) return rec1->FValue;
                    else if (rec1->Type!=CEePromRecord::Unknown) return rec1->IValue;
                }
            }
            else if (role==Qt::BackgroundRole){
                rec1 = Values.at(row);
                rec2 = Printer.EEPROM->at(row);
                if (rec1 && rec2){
                    if (!rec1->isEqual(rec2))
                        return QBrush(Qt::red);
                }
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags QEepromTableModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags fl = QAbstractTableModel::flags(index);
    if (index.isValid() && index.column()==0){
        int  row = index.row();
        if (row!=0) return fl | Qt::ItemIsEditable;
    }
    return fl;
}

QVariant QEepromTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
//        if (section==0) return tr("Description");
        if (section==0) return tr("Value");
    }
    else if (orientation == Qt::Vertical && role == Qt::DisplayRole){
        if (section>=0 && section<Values.count()){
            const CEePromRecord* rec = Values.at(section);
            return rec->Description;

        }
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void QEepromTableModel::update()
{
    beginResetModel();
    Values.clear();
    for (int i=0,n=Printer.EEPROM->count();i<n;i++){
        Values.append(Printer.EEPROM->at(i));
    }
    endResetModel();
}

bool QEepromTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (index.isValid() && role==Qt::EditRole){
        CEePromRecord* rec = Values.at(index.row());
        if (rec){
            if (rec->Type==CEePromRecord::Float)
                rec->FValue = value.toFloat();
            else rec->IValue = value.toInt();
            emit dataChanged(index,index);
            return true;
        }
    }
    return false;
}

/*****************************************************************************\
*                                                                             *
*                                                                             *
\*****************************************************************************/
CEepromConfiguration::CEepromConfiguration(QWidget *parent) :
    CBaseWindow(parent),Ui::eeprom_conf()
{
    setupUi(this);
    setWindowTitle(tr("EEPROM"));
    setWindowIcon(QIcon(":/images/chip.png"));
    Model = new QEepromTableModel();
    tableView->setModel(Model);
    tableView->setItemDelegate(new QEepromDelegate(this));
    updateModel = true;
    connect (Printer.EEPROM,SIGNAL(signalReady(QString)),this,SLOT(slotReady(QString)));
}

CEepromConfiguration::~CEepromConfiguration()
{
}

void CEepromConfiguration::on_pbRefresh_clicked()
{
    Model->update();
}

void CEepromConfiguration::slotReady(const QString& name)
{
    if (name == CEeProm::TaskName){
        if (updateModel==true)
            Model->update();
        updateModel = false;
    }
}

void CEepromConfiguration::on_pbRead_clicked()
{
    updateModel = true;
    Printer.EEPROM->read();
}

void CEepromConfiguration::on_pbWrite_clicked()
{
    QStringList script;
    for (int i=0,n=Model->count();i<n;i++){
        CEePromRecord* rec1 = Model->at(i);
        CEePromRecord* rec2 = Printer.EEPROM->at(i);
        if (!rec1->isEqual(rec2))
            script.append(rec1->ToCmdString());
        if (rec1->Type==CEePromRecord::Float)
            Printer.EEPROM->setParameterValue(i,rec1->FValue);
        else
            Printer.EEPROM->setParameterValue(i,rec1->IValue);
    }
    updateModel = true;
    Printer.playScript(script);
}

void CEepromConfiguration::on_pbSave_clicked()
{
//    Model->update();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Select file name"),"",
                                                    tr("Txt-files (*.txt);;All files (*.*)"));
    if (!fileName.isEmpty()){
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setInformativeText(tr("Error"));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            QTextStream stream(&file);
            try{
                for (int i=0,n=Model->count();i<n;i++){
                    CEePromRecord* rec = Model->at(i);
                    if (rec)
                        stream << rec->ToString() << endl;
                }
            }
            catch(...){
                mb.setText(file.errorString());
                mb.exec();
            }
        }
        else{
            mb.setText(file.errorString());
            mb.exec();
        }
    }

}

void CEepromConfiguration::on_pbLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select file name"),"",
                                                    tr("Txt-files (*.txt);;All files (*.*)"));
    if (!fileName.isEmpty()){
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setInformativeText(tr("Error"));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);

        QString line;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)){
            QTextStream stream(&file);
            Model->clear();
            try{
                while (!stream.atEnd()){
                    line = stream.readLine().trimmed();
                    if (!line.isEmpty()){
                        CEePromRecord* rec = new CEePromRecord();
                        if (rec->FromString(line))
                            Model->append(rec);
                        else delete rec;
                    }
                }
            }
            catch(...){
                mb.setText(file.errorString());
                mb.exec();
            }
        }
        else{
            mb.setText(file.errorString());
            mb.exec();
        }
    }
}
