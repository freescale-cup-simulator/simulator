#ifndef MODEL_H
#define MODEL_H

#include <QAbstractTableModel>
#include <QVector2D>
#include <QDebug>

#include <property.h>

class SettingsModel;

SettingsModel * getSettingsModelInstance();

class SettingsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SettingsModel(QObject *parent = nullptr);
    enum PropertyRoles
    {
        NameRole = Qt::UserRole + 1,
        ValueRole
    };

    int rowCount(const QModelIndex &parent  = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void deleteProperty(const QString &propName);
    double getPropertyValue(const QString &propName);
    void addProperty(Property prop);

public slots:
    void valueChanged(double value, int row);

signals:
    void internalAddPropSignal(Property prop);

private slots:
    void internalAddPropSlot(Property prop);

private:
    QVector<Property> m_props;

};

#endif // MODEL_H
