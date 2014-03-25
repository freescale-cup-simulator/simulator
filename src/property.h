#ifndef PROPERTY_H
#define PROPERTY_H

#include <QString>
#include <QVariant>

class Property
{
public:
    Property() { }
    Property(const QString &nm, const double val);

    QString name() const;
    double value() const;

    void setValue(double val);
    bool operator ==(Property prop);

private:
    QString m_name;
    double m_value;


};

Q_DECLARE_METATYPE(Property)

#endif // PROPERTY_H
