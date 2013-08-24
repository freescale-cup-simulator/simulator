#ifndef ICONTROLALGORITHM_H
#define ICONTROLALGORITHM_H

#include <Common.h>
#include <QByteArray>

class IControlAlgorithm : public QObject
{
    Q_OBJECT
public:
    IControlAlgorithm(QObject * parent = nullptr)
        : QObject(parent)
    {
    }
    virtual QString getId() = 0;
    virtual void setInterval(int msec) = 0;
public slots:
    virtual void onCameraResponse(const QByteArray & frame) = 0;
signals:
    void controlSignal(const DataSet & data);
};

#endif // ICONTROLALGORITHM_H
