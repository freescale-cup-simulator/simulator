#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#include <QSettings>

class QString;
class QVariant;

/*!
 * \brief A simple QSettings wrapper to be made visible to QML
 */
class UserSettings : public QObject
{
    Q_OBJECT
public:
    UserSettings(QObject * parent = nullptr)
        : QObject(parent) {}

    Q_INVOKABLE void setValue(const QString & key, const QVariant & value)
    {
        m_settings.setValue(key, value);
    }

    Q_INVOKABLE QVariant value(const QString & key)
    {
        return m_settings.value(key, QVariant());
    }

    Q_INVOKABLE bool contains(const QString & key)
    {
        return m_settings.contains(key);
    }

private:
    QSettings m_settings;
};

#endif // USER_SETTINGS_H
