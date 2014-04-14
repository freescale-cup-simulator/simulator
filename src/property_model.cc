#include <property_model.h>

PropertyModel::PropertyModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    QObject::connect(this, &PropertyModel::internalAddPropSignal,
                     this, &PropertyModel::internalAddPropSlot,
                     Qt::QueuedConnection);
}

int PropertyModel::rowCount(const QModelIndex &) const
{
        return m_props.count();
}

int PropertyModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant PropertyModel::data(const QModelIndex &index, int role) const
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

QHash<int, QByteArray> PropertyModel::roleNames() const
{
    static QHash<int, QByteArray> roles
    {
        { NameRole,  "name"  },
        { ValueRole, "value" }
    };

    return roles;
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return Qt::ItemIsEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

void PropertyModel::addProperty(Property prop)
{
    if (!m_props.contains(prop))
        emit internalAddPropSignal(prop);
}

void PropertyModel::internalAddPropSlot(Property prop)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_props << prop;
    endInsertRows();
}

void PropertyModel::deleteProperty(const QString & propName)
{
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    for (int i = 0; i < m_props.count(); i++)
        if (m_props.at(i).name() == propName)
            m_props.removeAt(i);

    endRemoveRows();
}

double PropertyModel::getPropertyValue(const QString & propName)
{
    for (int i = 0; i < m_props.count(); i++)
        if (m_props.at(i).name() == propName)
            return m_props.at(i).value();
}

void PropertyModel::valueChanged(double value, int row)
{
    qDebug() << row << value;
    m_props[row].setValue(value);
}
