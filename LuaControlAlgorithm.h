#ifndef LUACONTROLALGORITHM_H
#define LUACONTROLALGORITHM_H

#include <IControlAlgorithm.h>
#include <lua.hpp>

class LuaControlAlgorithm : public IControlAlgorithm
{
    Q_OBJECT
public:
    LuaControlAlgorithm(QObject * parent = nullptr);
    ~LuaControlAlgorithm();

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
    void init();
signals:
    void unloaded();
};

#endif // LUACONTROLALGORITHM_H
