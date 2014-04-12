#include <property.h>

Property::Property(const QString &name, double default_value,
                   double value_step)
    : m_name(name)
    , m_value(default_value)
    , m_value_step(value_step)
{}

QString Property::name() const
{
    return m_name;
}

double Property::value() const
{
    return m_value;
}

double Property::value_step() const
{
    return m_value_step;
}

void Property::setValue(double val)
{
    m_value = val;
}

bool Property::operator ==(const Property & prop)
{
    return (prop.name() == this->name());
}
