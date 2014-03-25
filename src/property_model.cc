#include "property_model.h"

SettingsModel * getSettingsModelInstance()
{
    static SettingsModel model;

    return &model;
}

SettingsModel::SettingsModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    QObject::connect(this, &SettingsModel::internalAddPropSignal,
                     this, &SettingsModel::internalAddPropSlot,
                     Qt::QueuedConnection);
}

int SettingsModel::rowCount(const QModelIndex &parent) const
{
        return m_props.count();
}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const Property &prop = m_props[index.row()];
    if (role == NameRole)
        return prop.name();
    else if (role == ValueRole)
        return prop.value();
    return QVariant();
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "SetData";
    return false;
}

QHash<int, QByteArray> SettingsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    return roles;
}


Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    if(index.column()==0)
        return Qt::ItemIsEnabled;
    else
        return Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable;
}

void SettingsModel::addProperty(Property prop)
{
    if(!m_props.contains(prop))
        emit internalAddPropSignal(prop);
}

void SettingsModel::internalAddPropSlot(Property prop)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_props << prop;
    endInsertRows();
}

void SettingsModel::deleteProperty(QString propName)
{
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    for(int i = 0; i < m_props.count(); i++)
        if(m_props.at(i).name() == propName)
            m_props.removeAt(i);

    endRemoveRows();
}

double SettingsModel::getPropertyValue(QString propName)
{
    for(int i = 0; i < m_props.count(); i++)
        if(m_props.at(i).name() == propName)
            return m_props.at(i).value();
}

void SettingsModel::valueChanged(double value, int row)
{
    qDebug() << "value: " << value << "row: " << row; //<< endl;
    m_props[row].setValue(value);
}
