#ifndef PROPERTY_H
#define PROPERTY_H

#include <QString>
#include <QObject>

/*!
 * \brief Represents a single user-defined property
 */
class Property
{
public:
    Property() {}
    Property(const QString &name, double default_value,
             double value_step = 0.1);

    QString name() const;
    double value() const;
    double value_step() const;

    void setValue(double val);
    bool operator ==(const Property &prop);

private:
    QString m_name;
    double m_value;
    double m_value_step;
};

Q_DECLARE_METATYPE(Property)

#endif // PROPERTY_H
