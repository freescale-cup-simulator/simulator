#ifndef CONTROL_ALGORITHM_H
#define CONTROL_ALGORITHM_H

#include <QObject>
#include <QFileInfo>
#include <QDebug>

#include <lua.hpp>
#include <common.h>
#include <config.h>

/*!
 * \brief Interface to user-created Lua control algorithms
 */
class ControlAlgorithm : public QObject
{
    Q_OBJECT
public:
    ControlAlgorithm(QObject * parent = nullptr);
    ~ControlAlgorithm();

    /*!
     * \brief Load a control algorithm from a file
     * \param file path to the algorithm file
     * \return true on success, false otherwise
     */
    bool loadFile(const QString & file);
    /*!
     * \brief Identify currently loaded algorithm
     * \return the name of currently loaded algorithm file
     */
    QString getId();
    /*!
     * \brief Pass the DataSet to Lua and recieve control signals
     * \param data the DataSet to process; returned control signals from Lua
     *        are placed here as well
     * \param frame line scan camera frame
     */
    void process(DataSet & data, quint8 frame[]);
private:
    /*!
     * \brief Utility: add a directory to Lua search path, so user scripts can
     *        include multiple files
     */
    void appendToLuaPath(const QString &path);

    lua_State * m_lua_state;
    QString m_current_file;
    bool m_lua_loaded;
};

#endif // CONTROL_ALGORITHM_H
