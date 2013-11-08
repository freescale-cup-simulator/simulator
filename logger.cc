#include "logger.h"

Logger::Logger ()
    : m_stream(0)
    , m_file(0)
    , m_mode(Logger::Closed)
{

}

void Logger::setFileName(const QString &filename)
{
    if(!m_file)
        m_file=new QFile(filename);
    else
        m_file->setFileName(filename);
}

bool Logger::beginWrite()
{
    Q_ASSERT(m_file);
    if(m_file->open(QIODevice::WriteOnly))
    {
        m_mode=Logger::Write;
        m_stream.setDevice(m_file);
        m_stream<<DATASET_VERSION;
        return true;
    }
    else
        return false;
}

void Logger::endWrite()
{
    m_stream.setDevice(0);
    m_file->close();
    m_mode=Logger::Closed;
}

bool Logger::beginRead()
{
    Q_ASSERT(m_file);
    if(m_file->open(QIODevice::ReadOnly))
    {
        m_stream.setDevice(m_file);
        quint32 header=0;
        m_stream>>header;
        if(header!=DATASET_VERSION)
        {
            m_stream.setDevice(0);
            m_file->close();
            log("Bad log file or different version.");
            return false;
        }
        if((m_file->size()-4)%sizeof(DataSet)!=0)
        {
            log("Warning! May be corrupted data in logfile.");
        }
        m_mode=Logger::Read;
        return true;
    }
    else
        return false;
}

void Logger::endRead()
{
    endWrite();
}

bool Logger::seek(quint64 blocksFromStart)
{
    if(m_mode!=Logger::Read)
    {
        log("Seek function available only in Read mode.");
        return false;
    }
    return m_file->seek(4+sizeof(DataSet)*blocksFromStart);
}

quint64 Logger::size()
{
    if(m_mode!=Logger::Read)
    {
        log("Size function available only in Read mode.");
        return false;
    }
    return (m_file->size()-4)/sizeof(DataSet);
}

Logger & Logger::operator <<(DataSet &dataset)
{
    if(m_mode!=Logger::Write)
    {
        log("Can't write. Wrong openMode or closed file.");
        return *this;
    }
    m_stream.writeRawData((char *)dataset.camera_pixels,CAMERA_FRAME_LEN);
    m_stream<<dataset.camera_position;
    m_stream<<dataset.camera_rotation;
    m_stream<<dataset.camera_rotation_quat;
    m_stream<<dataset.control_interval;
    m_stream<<dataset.current_wheel_angle;
    m_stream<<dataset.desired_wheel_angle;
    m_stream<<dataset.physics_timestep;
    m_stream<<dataset.wheel_power_l;
    m_stream<<dataset.wheel_power_r;
    return *this;
}

Logger &Logger::operator >>(DataSet &dataset)
{
    if(m_mode!=Logger::Read)
    {
        log("Can't read. Wrong openMode or closed file.");
        return *this;
    }
    m_stream.readRawData((char *)dataset.camera_pixels,CAMERA_FRAME_LEN);
    m_stream>>dataset.camera_position;
    m_stream>>dataset.camera_rotation;
    m_stream>>dataset.camera_rotation_quat;
    m_stream>>dataset.control_interval;
    m_stream>>dataset.current_wheel_angle;
    m_stream>>dataset.desired_wheel_angle;
    m_stream>>dataset.physics_timestep;
    m_stream>>dataset.wheel_power_l;
    m_stream>>dataset.wheel_power_r;
    if(m_stream.status()==QDataStream::ReadCorruptData)
        throw CorruptedStructureException();
    return *this;
}

void Logger::log(QString text)
{
    qDebug()<<"[Logger] "<<text;
}


Logger::~Logger()
{
    m_file->close();
    delete m_file;
}
