#ifndef QLOGER_H
#define QLOGER_H
#include <QFile>
#include <QDataStream>
#include <QVector>
#include "common.h"

class QLogger
{
    private:
        QFile * file;
        QDataStream stream;
        void closeLogger();
    public:
        QLogger(QString fileName);
        void read(DataSet & dataSetBuffer );
        void write(DataSet & dataSet);
        QVector <DataSet> * readAllFile();
        ~QLogger();
};
#endif // QLOGER_H
