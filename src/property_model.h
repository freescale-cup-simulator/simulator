#ifndef MODEL_H
#define MODEL_H

#include <QAbstractTableModel>
#include <QDebug>
#include "property.h"

class SettingsModel;

SettingsModel * getSettingsModelInstance();

class SettingsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SettingsModel(QObject *parent = 0);
    enum PropertyRoles {
        NameRole = Qt::UserRole + 1,
        ValueRole
    };

    int rowCount(const QModelIndex &parent  = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QHash<int, QByteArray> roleNames() const;


    void deleteProperty(QString propName);
    double getPropertyValue(QString propName);
    void addProperty(Property prop);

public slots:
    void valueChanged(double value, int row);

signals:
    void internalAddPropSignal(Property prop);

private slots:
    void internalAddPropSlot(Property prop);



private:
    QVector<Property> m_props;

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // MODEL_H
