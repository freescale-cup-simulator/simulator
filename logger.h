#ifndef LOGER_H
#define LOGER_H

#include <QFile>
#include <QDataStream>
#include <QVector>
#include <QString>
#include <exception>

#include "common.h"

class Logger
{

public:

    enum Mode{Closed,Read,Write};

    Logger();

    void setFileName(const QString &filename);

    bool beginWrite();
    void endWrite();

    bool beginRead();
    void endRead();

    Mode mode() {return m_mode;}
    quint64 size();

    Logger & operator <<(DataSet & dataset);
    Logger & operator >> (DataSet & dataset);

    bool seek(quint64 blocksFromStart);
    ~Logger();

    class CorruptedStructureException {};
private:

    QFile * m_file;
    QDataStream m_stream;
    Mode m_mode;
    void log(QString text);

};
#endif // LOGER_H
