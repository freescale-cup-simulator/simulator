#ifndef LOGER_H
#define LOGER_H

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QDebug>

#include <H5Cpp.h>

#include <common.h>


/*!
 * \brief Saves simulation data for further analysis
 */

class Logger
{
public:
    void appendDataset(const DataSet & ds);
    bool saveToFile(const QString & fileName, bool clearAfter = true);

    int currentLogSize() { return m_dataset_vector.size(); }

    void clear()
    {
        qDeleteAll(m_dataset_vector);
        m_dataset_vector.clear();
    }

private:
    typedef QVector<float *> DatasetVector;
    static constexpr int HDF_DATASET_RANK = 2;

    DatasetVector m_dataset_vector;
};
#endif // LOGER_H
