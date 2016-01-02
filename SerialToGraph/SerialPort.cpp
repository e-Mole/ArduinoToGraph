#include "SerialPort.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QString>
#include <string>


#define RESPONSE_WAITING 100 //100 ms should be enough

ExtendedSerialPortInfo::ExtendedSerialPortInfo(QSerialPortInfo const &info, QSettings const &settings) :
    QSerialPortInfo(info),
    m_preferred(false),
    m_lastUsed(false)
{
    if (info.portName() == settings.value("lastSerialPort", ""))
    {
        m_preferred = true;
        m_lastUsed = true;
    }
    else if (info.manufacturer() == "wch.cn")
    {
        qDebug() << info.portName() << "looks like my port";
        m_preferred = true;
    }
}

SerialPort::SerialPort(QSettings &settings, QObject *parent) :
    QObject(parent),
    m_settings(settings),
    m_knownIssue(false)
{

}

SerialPort::~SerialPort()
{
    if (m_serialPort.isOpen())
        Stop();
}

bool SerialPort::OpenSerialPort(QSerialPortInfo const& info)
{
    if (m_serialPort.isOpen())
    {
        m_serialPort.close();
        PortConnectivityChanged(false);
    }

    m_serialPort.setPort(info);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);

    if (m_serialPort.open(QIODevice::ReadWrite))
    {
        QThread::sleep(3); //arduino is reseted after serial port connection I have to wait to be ready

        Write(INS_GET_VERSION, "");
        QByteArray array;
        unsigned counter = RESPONSE_WAITING;
        while (!m_serialPort.waitForReadyRead(1))
        {
            if (0 == --counter)
            {
                qDebug() << "no response from serial port";
                m_serialPort.close();
                return false;
            }
        }

        ReadAll(array);
        if (array.toStdString() != "ATG_1")
        {
            qDebug() << "unknown protocol version";
            m_serialPort.close();
            return false;
        }

        PortConnectivityChanged(true);
        m_knownIssue = false; //connection is estabilished. Connection fail will be a new issue.
        return true;
    }

    qDebug() << "unable to open port " << info.portName();
    return false;
}

bool SerialPort::FindAndOpenMySerialPort(QList<ExtendedSerialPortInfo> &portInfos)
{
    QList<QSerialPortInfo> prefferedPortInfos;
    ExtendedSerialPortInfo *lastPort = NULL;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        portInfos.push_back(ExtendedSerialPortInfo(info, m_settings));

        if (portInfos.last().m_preferred)
            prefferedPortInfos.push_back(info);
        if (portInfos.last().m_lastUsed)
            lastPort = &portInfos.last();
    }

    if (NULL != lastPort && OpenSerialPort(*lastPort))
        return true;

    if (1 == prefferedPortInfos.size() && OpenSerialPort(prefferedPortInfos.last()))
        return true;

    if (portInfos.empty())
    {
        qDebug() << "hardware not found";
        PortIssueSolver();
        return false;
    }

    if (prefferedPortInfos.size() > 1)
        qDebug() << "more then one preferred port found";
    else if (!portInfos.empty())
        qDebug() << "found unpreffered serial ports only";

    return false;
}

void SerialPort::ReadAll(QByteArray &array)
{
    array = m_serialPort.readAll();
}

void SerialPort::PortIssueSolver()
{
	m_serialPort.close();

    if (!m_knownIssue)
    {
        m_knownIssue = true;
        QMessageBox::warning(
            NULL,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("You are working in an offline mode. To estabilish a connection, please, reconnect the device and restart the application.")
        );
    }

	PortConnectivityChanged(false);
}

bool SerialPort::Write(Instructions instruction, std::string const &data)
{
	if (!m_serialPort.isOpen())
        return false;

    qDebug() << "writen instruction:" << instruction << " data size:" <<
        m_serialPort.write((char const *)&instruction , 1);
	if (!m_serialPort.waitForBytesWritten(RESPONSE_WAITING))
	{
		//_LineIssueSolver();
		//return;
	}
    if (data.size() > 0)
	{
       qDebug() << "data present" << data.c_str() << " size:" << data.size();
       m_serialPort.write(data.c_str(), data.size());
	  if (!m_serialPort.waitForBytesWritten(RESPONSE_WAITING))
	  {
		//_LineIssueSolver();
	  }
    }
    return true;
}

bool SerialPort::SetFrequency(unsigned frequency)
{
	std::string tmp;
    tmp.append((char const *)&frequency, 2);
    if (!Write(INS_SET_FREQUENCY, tmp))
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool SerialPort::SetTime(unsigned time)
{
    std::string tmp;
    tmp.append((char const *)&time, 2);
    if (!Write(INS_SET_TIME, tmp))
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool SerialPort::Start()
{
    if (!Write(INS_START, ""))
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool SerialPort::Stop()
{
    if (!Write(INS_STOP, ""))
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

void SerialPort::SetSelectedChannels(unsigned char channels)
{
    std::string tmp;
    tmp.append((char const *)&channels, 1);
    Write(INS_ENABLED_CHANNELS, tmp);
    //Write(INS_ENABLED_CHANNELS, tmp);
}

bool SerialPort::IsDeviceConnected()
{
    if (!m_serialPort.isOpen())
    {
        PortIssueSolver();
        return false;
    }
    return true;
}
