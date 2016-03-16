#include "SerialPort.h"
#include <GlobalSettings.h>
#include <hw/PortInfo.h>
#include <QList>
#include <QThread>
#include <QString>
#include <QTimer>
#include <string>
#include <hw/HwSink.h>

#define RESPONSE_WAITING 100 //100 ms should be enough

namespace hw
{

SerialPort::SerialPort(GlobalSettings &settings, HwSink *hwSink) :
    PortBase(hwSink),
    m_settings(settings),
    m_hwSink(hwSink)
{
    connect(&m_serialPort, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
}

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::_OpenPort(QSerialPortInfo const &info)
{
    m_serialPort.setPort(info);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);

    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        qDebug() << "unable to open port " << info.portName();
        portOpeningFinished();
        return false;
    }

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(portOpenTimeout()));
    timer->start(3000); //arduino is reseted after serial port connection I have to wait to be ready
    return true;
}

void SerialPort::portOpenTimeout()
{
    portOpeningFinished();
    return;
}

bool SerialPort::OpenPort(QString id)
{
    auto it = m_idToInfo.find(id);
    if (it == m_idToInfo.end())
        return false;

    return _OpenPort(it.value());
}

bool SerialPort::IsOpen()
{
    return m_serialPort.isOpen();
}

void SerialPort::FillPorts(QList<PortInfo> &portInfos)
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QString id = info.portName();
        if (!info.description().isEmpty())
            id += " (" + info.description() + ")";
        else if (!info.manufacturer().isEmpty())
            id += " (" + info.manufacturer() + ")";

        portInfos.push_back(
            PortInfo(
                PortInfo::pt_serialPort,
                id,
                info.manufacturer() == "wch.cn",
                m_settings
            )
        );

        m_idToInfo[id] = info;
    }
}

qint64 SerialPort::Write(char const *data, unsigned size)
{
    return m_serialPort.write(data, size);
}

void SerialPort::WaitForBytesWritten()
{
    m_serialPort.waitForBytesWritten(RESPONSE_WAITING);
}
void SerialPort::ReadData(QByteArray &array, unsigned maxLength)
{
    /*unsigned counter = RESPONSE_WAITING;
    while (--counter > 0 && !m_serialPort.waitForReadyRead(100)) //100 to be sure I get response from baudrate 9600 too
    {
    }*/

    array = m_serialPort.read(maxLength);
}

void SerialPort::ReadData(QByteArray &array)
{
    array = m_serialPort.readAll();
}

void SerialPort::Close()
{
    if (m_serialPort.isOpen())
        m_serialPort.close();
}

} //namespace hw
