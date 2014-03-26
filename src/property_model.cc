#include <property_model.h>

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

int SettingsModel::rowCount(const QModelIndex &) const
{
        return m_props.count();
}

int SettingsModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Property & prop = m_props[index.row()];
    if (role == NameRole)
        return prop.name();
    else if (role == ValueRole)
        return QVector2D(prop.value(), prop.value_step());

    return QVariant();
}

QHash<int, QByteArray> SettingsModel::roleNames() const
{
    static QHash<int, QByteArray> roles
    {
        { NameRole,  "name"  },
        { ValueRole, "value" }
    };

    return roles;
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return Qt::ItemIsEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void SettingsModel::addProperty(Property prop)
{
    if (!m_props.contains(prop))
        emit internalAddPropSignal(prop);
}

void SettingsModel::internalAddPropSlot(Property prop)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_props << prop;
    endInsertRows();
}

void SettingsModel::deleteProperty(const QString & propName)
{
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    for (int i = 0; i < m_props.count(); i++)
        if (m_props.at(i).name() == propName)
            m_props.removeAt(i);

    endRemoveRows();
}

double SettingsModel::getPropertyValue(const QString & propName)
{
    for (int i = 0; i < m_props.count(); i++)
        if (m_props.at(i).name() == propName)
            return m_props.at(i).value();
}

void SettingsModel::valueChanged(double value, int row)
{
    qDebug() << row << value;
    m_props[row].setValue(value);
}
