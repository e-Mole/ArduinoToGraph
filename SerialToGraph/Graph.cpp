#include "Graph.h"

#include <Axis.h>
#include <cmath>
#include <Channel.h>
#include <Context.h>
#include <limits.h>
#include <math.h>
#include <MyCustomPlot.h>
#include <QBoxLayout>
#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QHBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QTimer>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>
#include <SerialPort.h>


#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 5

Graph::Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar) :
    QWidget(parent),
    m_context(context),
    m_plot(NULL),
    m_serialPort(serialPort),
    m_period(0),
    m_counter(0),
    m_scrollBar(scrollBar),
    m_periodTypeIndex(0),
    m_connectButton(NULL),
    m_sampleChannel(NULL),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_anySampleMissed(false),
    m_drawingRequired(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    QHBoxLayout *documentLayout = new QHBoxLayout(this);
    mainLayout->addLayout(documentLayout);
    QVBoxLayout *graphLayout = new QVBoxLayout(this);
    documentLayout->addLayout(graphLayout);

    _InitializePolt(graphLayout);

    m_drawTimer = new QTimer(this);
    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));
}

void Graph::_InitializePolt(QBoxLayout *graphLayout)
{
    m_plot = new MyCustomPlot(this, m_context);
    graphLayout->addWidget(m_plot);

}

void Graph::periodTypeChanged(int index)
{
        m_periodTypeIndex = index;
}

void Graph::_FillGraphItem(GraphItem &item)
{
    //FIXME: firstInSample is not used any more. remove it
    unsigned char mixture = m_queue.dequeue();
    item.afterThrownOutSample = ((mixture >> 6) & 1);
    item.channelIndex = mixture & 7; //lowest 3 bits
	char value[4];
	value[0] = m_queue.dequeue();
	value[1] = m_queue.dequeue();
	value[2] = m_queue.dequeue();
	value[3] = m_queue.dequeue();

	item.value = *((float*)value);
}

bool Graph::_FillQueue()
{
    QByteArray array;
    m_serialPort.ReadAll(array);
    if (array.size() == 0)
        return false; //nothing to fill

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    return true;
}

bool Graph::_IsCompleteSetInQueue()
{
    return m_queue.size() >= m_trackedHwChannels.size() * CHANNEL_DATA_SIZE;
}

void Graph::draw()
{
    m_plot->SetDrawingInProcess(true);

    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (_FillQueue() && _IsCompleteSetInQueue())
    {
    
        while (_IsCompleteSetInQueue())
        {
            m_sampleChannel->AddValue(m_sampleChannel->GetValueCount());

            GraphItem item;
            for (int i = 0; i < m_trackedHwChannels.size(); i++) //i is not used. just for right count of reading from the queue
            {
                _FillGraphItem(item);

                if (item.afterThrownOutSample)
                {
                    m_anySampleMissed = true;
                    qDebug() << "writing delay";
                }

                if (m_trackedHwChannels[item.channelIndex]->GetAxis()->IsHorizontal() && item.value <= m_trackedHwChannels[item.channelIndex]->GetMaxValue())
                    qDebug() << "vale is less then max";
                m_trackedHwChannels[item.channelIndex]->AddValue(item.value);
            }

            m_sampleChannel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());
            foreach (Channel *channel, m_trackedHwChannels)
                channel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());
        }

        unsigned index = m_plot->GetHorizontalChannel()->GetValueCount() - 1;
        if (!m_plot->IsInMoveMode())
        {

            foreach (Channel *channel, m_context.m_channels)
                channel->displayValueOnIndex(index);

            m_plot->RescaleAxis(m_plot->xAxis);
        }

        unsigned scrollBarMax = index;
        m_scrollBar->setRange(0, scrollBarMax);
        if (!m_plot->IsInMoveMode())
        {
            m_scrollBar->setValue(scrollBarMax);
        }

        m_plot->RescaleAllAxes();
        m_plot->ReplotIfNotDisabled();
    }

    _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));
    if (m_drawingRequired)
        m_drawTimer->start(m_drawPeriod);

    m_plot->SetDrawingInProcess(false);
}

void Graph::FinishDrawing()
{
    m_drawingRequired = false;
    m_drawTimer->stop();
    m_plot->WaitForDrawingIsFinished();
}

void Graph::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > 500) //delay will be still longer, I have to stop drawing for this run
            m_plot->SetDisabled(true);
        else
        {
            m_drawPeriod *= 2;
            qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
}

void Graph::start()
{
    if (!m_serialPort.IsDeviceConnected())
    {
        m_serialPort.LineIssueSolver();
        return;
    }

    m_anySampleMissed = false;
    m_counter = 0;

    m_trackedHwChannels.clear();
    foreach (Channel *channel, m_context.m_channels)
    {
        channel->ClearValues();
        if (channel->IsHwChannel() && !channel->isHidden())
            m_trackedHwChannels.insert(channel->GetHwIndex(), channel);
    }

    for (int i = 0; i< m_plot->graphCount(); i++)
        m_plot->graph(i)->data()->clear();

    m_queue.clear();
    m_serialPort.Clear(); //throw buffered data avay. I want to start to listen now

    if (0 == m_periodTypeIndex)
    {
        if (!m_serialPort.SetFrequency(m_period))
        {
            m_serialPort.LineIssueSolver();
            return;
        }

        qDebug() << "frequency set to:" << m_period << " Hz";
    }
    else
    {
        if (!m_serialPort.SetTime(m_period))
        {
            m_serialPort.LineIssueSolver();
            return;
        }
        qDebug() << "time set to:" << m_period << " s";
    }

    unsigned selectedChannels = 0;
    foreach (Channel *channel, m_trackedHwChannels.values())
        selectedChannels |= 1 << channel->GetHwIndex();

    qDebug() << "selected channels:" << selectedChannels;
    m_serialPort.SetSelectedChannels(selectedChannels);
    m_scrollBar->setRange(0, 0);

    m_drawPeriod = INITIAL_DRAW_PERIOD;
    m_drawingRequired = true;
    m_plot->ContinueDrawing();
    m_drawTimer->start(m_drawPeriod);
    if (!m_serialPort.Start())
    {
        m_serialPort.LineIssueSolver();
        return;
    }
}

void Graph::stop()
{
    if (!m_serialPort.Stop())
        qDebug() << "stop was not deliveried";

    FinishDrawing();

    m_plot->SetDisabled(false);
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_plot->SetDisabled(false);
    if (m_anySampleMissed)
        QMessageBox::warning(
            this,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("Some samples was not delivered. The sample rate is probably too high for so many channels.")
        );
}

void Graph::exportPng(QString const &fileName)
{
    m_plot->savePng(fileName);
}

void Graph::exportCsv(QString const &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    unsigned lineNr = 0;

    file.write(m_plot->xAxis->label().toStdString().c_str());
	file.write(";");
    for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
    {
        file.write(m_context.m_channels[i]->GetName().toStdString().c_str());
        if (i == (unsigned)m_context.m_channels.size() - 1)
             file.write("\n");
        else
             file.write(";");
    }

	unsigned sampleNr = 0;
    while (true)
    {
        bool haveData = false;
		std::string lineContent = QString("%1;").arg(sampleNr++).toStdString();
        for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
        {
            if (m_context.m_channels[i]-> GetValueCount() > lineNr)
            {
                lineContent.append(QString("%1").arg(m_context.m_channels[i]->GetValue(lineNr)).toStdString());
                haveData = true;
            }

            if (i == (unsigned)m_context.m_channels.size() - 1)
                lineContent.append("\n");
            else
                lineContent.append(";");
        }

        lineNr++;

        if (haveData)
            file.write(lineContent.c_str(), lineContent.size());
        else
            break;
    }
    file.close();
}

void Graph::periodChanged(unsigned period)
{
    m_period = period;
}

void Graph::sliderMoved(int value)
{
    foreach (Channel * channel, m_context.m_channels)
        channel->displayValueOnIndex(value);

    m_plot->SetMoveMode(true);
    m_plot->ReplotIfNotDisabled();
}

QCPGraph *Graph::AddGraph(QColor const &color)
{
    return m_plot->AddGraph(color);
}

QCPGraph *Graph::AddPoint(QColor const &color, unsigned shapeIndex)
{
    return m_plot->AddPoint(color, shapeIndex);
}

void Graph::SetSampleChannel(Channel *channel)
{
    m_sampleChannel = channel;
}

void Graph::SetHorizontalChannel(Channel *channel)
{
    m_plot->SetHorizontalChannel(channel);
}

MyCustomPlot *Graph::GetPlot()
{
    return m_plot;
}
