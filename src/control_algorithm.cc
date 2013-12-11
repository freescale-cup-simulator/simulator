#include <control_algorithm.h>

ControlAlgorithm::ControlAlgorithm(QObject *parent)
    : QObject(parent)
    , m_lua_state(luaL_newstate())
    , m_lua_loaded(false)
{
    luaL_openlibs(m_lua_state);
}

ControlAlgorithm::~ControlAlgorithm()
{
    lua_close(m_lua_state);
}

bool ControlAlgorithm::loadFile(const QString &file)
{
    Q_ASSERT(!m_lua_loaded);

    m_lua_loaded = (luaL_loadfile(m_lua_state, file.toLocal8Bit().data()) == 0);
    if (!m_lua_loaded)
    {
        qWarning() << "Load error: " << lua_tostring(m_lua_state, -1);
        lua_pop(m_lua_state, 1);
        return false;
    }

    lua_pushvalue(m_lua_state, -1);
    lua_pcall(m_lua_state, 0, 0, 0);

    lua_pushnumber(m_lua_state, m_interval);
    lua_setglobal(m_lua_state, "g_run_interval");

    qDebug() << "Script successfully loaded";
    return true;
}

QString ControlAlgorithm::getId()
{
    return "Lua Control algoritgm (file: " + m_current_file + ")";
}

void ControlAlgorithm::process(DataSet & d)
{
    Q_ASSERT(m_lua_loaded);

    // important: make a copy of the loaded function, pcall will pop it
    lua_pushvalue(m_lua_state, -1);

    lua_newtable(m_lua_state);
    for (int i = 0; i < CAMERA_FRAME_LEN; i++)
    {
        lua_pushnumber(m_lua_state, i + 1);
        lua_pushnumber(m_lua_state, static_cast<unsigned char>(d.camera_pixels[i]));
        lua_settable(m_lua_state, -3);
    }
    lua_setglobal(m_lua_state, "g_camera_frame");

    if (lua_pcall(m_lua_state, 0, LUA_MULTRET, 0) != 0)
        qFatal("Lua error: %s", lua_tostring(m_lua_state, -1));

    lua_getfield(m_lua_state, -1, "angle");
    d.desired_wheel_angle = lua_tonumber(m_lua_state, -1);
    lua_pop(m_lua_state, 1);

    lua_getfield(m_lua_state, -1, "lspeed");
    d.wheel_power_r = lua_tonumber(m_lua_state, -1);
    lua_pop(m_lua_state, 1);

    lua_getfield(m_lua_state, -1, "rspeed");
    d.wheel_power_l = lua_tonumber(m_lua_state, -1);
    lua_pop(m_lua_state, 1);

    lua_getfield(m_lua_state, -1, "linepos");
    d.line_position = lua_tonumber(m_lua_state, -1);
    lua_pop(m_lua_state, 1);

    lua_pop(m_lua_state, 1);
}
