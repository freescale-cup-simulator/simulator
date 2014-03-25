#include "property.h"

Property::Property(const QString &nm, const double val)
    : m_name(nm), m_value(val)
{ }

QString Property::name() const
{
    return m_name;
}

double Property::value() const
{
    return m_value;
}

void Property::setValue(double val)
{
    m_value = val;
}

bool Property::operator ==(Property prop)
{
    if(prop.name() == this->name())
        return true;
    else
        return false;
}
