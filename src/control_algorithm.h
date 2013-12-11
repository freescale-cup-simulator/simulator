#ifndef CONTROL_ALGORITHM_H
#define CONTROL_ALGORITHM_H

#include <QObject>
#include <QDebug>
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
    void process(DataSet & data);
private:
    lua_State * m_lua_state;
    QString m_current_file;
    bool m_lua_loaded;
    int m_interval;
};

#endif // CONTROL_ALGORITHM_H
