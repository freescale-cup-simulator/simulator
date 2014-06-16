#include <logger.h>

void Logger::appendDataset(const DataSet &ds)
{
    float * cds = new DataSet;
    std::memcpy(cds, &ds, sizeof(DataSet));
    m_dataset_vector.append(cds);
}

bool Logger::saveToFile(const QString &fileName, bool clearAfter)
{
    try
    {
        const unsigned int size = m_dataset_vector.size();

        H5::Exception::dontPrint();

        H5::H5File file(fileName.toLocal8Bit().data(), H5F_ACC_TRUNC);

        hsize_t dim[HDF_DATASET_RANK] = {size, DataSetValues::VALUE_MAX};
        H5::DataSpace dataspace(HDF_DATASET_RANK, dim);
        H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);
        datatype.setOrder(H5T_ORDER_LE);

        H5::DataSet dataset = file.createDataSet("DataSet", datatype, dataspace);
        QSharedPointer<DataSet> contiguous_data(new DataSet[size]);

        for (unsigned int i = 0; i < size; i++)
        {
            std::memcpy(&contiguous_data.data()[i], m_dataset_vector.at(i),
                        sizeof(DataSet));
        }

        dataset.write(contiguous_data.data(), H5::PredType::NATIVE_FLOAT);
    }
    catch (H5::Exception * e)
    {
        e->printError();
        return false;
    }

    if (clearAfter)
        clear();

    return true;
}
