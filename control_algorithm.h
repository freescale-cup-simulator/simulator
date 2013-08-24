#ifndef CONTROL_ALGORITHM_H
#define CONTROL_ALGORITHM_H

#include <QObject>
#include <lua.hpp>
#include <common.h>

class ControlAlgorithm : public QObject
{
    Q_OBJECT
public:
    ControlAlgorithm(QObject * parent = nullptr);
    ~ControlAlgorithm();

    bool loadFile(const QString & file);
    QString getId();
    void setInterval(int msec);
private:
    bool executeControlScript(const QByteArray & frame);

    lua_State * m_lua_state;
    QString m_current_file;
    bool m_lua_loaded;
    int m_interval;
public slots:
    void onCameraResponse(const QByteArray & data);
signals:
    void controlSignal(const DataSet & data);
};

#endif // CONTROL_ALGORITHM_H
