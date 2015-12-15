#include "Channel.h"
#include <ChannelSettings.h>
#include <Axis.h>
#include <Context.h>
#include <cmath>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QString>
#include <limits>

QSize Channel::ValueLabel::GetLongestTextSize()
{
    return GetSize("-0.000e-00\n");
}

QSize Channel::ValueLabel::GetSize(QString const &text)
{
    QFontMetrics metrics(this->font());
    return  metrics.size(0, text);
}

void Channel::ValueLabel::resizeEvent(QResizeEvent * event)
{
    QFont font = this->font();
    QFontMetrics metrics(font);

    QSize size = GetLongestTextSize();
    qreal factor = qMin(
                (qreal)width() / ((qreal)size.width()*1.1),
                (qreal)height() / ((qreal)size.height()*1.1)
    );

    font.setPointSizeF(font.pointSizeF() * factor);
    setFont(font);
}

void Channel::ValueLabel::SetColor(const QColor &color)
{
    QPalette palette = this->palette();
    palette.setColor(foregroundRole(), color);
    setPalette(palette);
}

Channel::Channel(
    QWidget *parent, Context const & context, int hwIndex, QString const &name, QColor const &color,
    Axis * axis, unsigned shapeIndex, QCPGraph *graph, QCPGraph *graphPoint) :
    QGroupBox(name, parent),
    m_valueLabel(NULL),
    m_context(context),
    m_name(name),
    m_hwIndex(hwIndex),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axis(NULL), //will be assigned inside constructor
    m_shapeIndex(shapeIndex),
    m_graph(graph),
    m_graphPoint(graphPoint)
{
    m_context.m_channels.push_back(this);
    AssignToAxis(axis);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(4);
    setLayout(layout);

    m_valueLabel = new ValueLabel("", color, IsHwChannel(), this);
    layout->addWidget(m_valueLabel);

    _DisplayNAValue();
    _SetMinimumSize();
    _UpdateTitle();

    if (m_axis->IsHorizontal())
        _ShowOrHideGraphAndPoin(false);

}

Channel::~Channel()
{

}

void Channel::changeChannelVisibility(bool visible, bool signal)
{
    setVisible(visible);
    _ShowOrHideGraphAndPoin(m_axis->IsHorizontal() ? false : visible);
    if (signal)
        stateChanged();
}

QString Channel::GetName()
{
    return m_name;
}

QString Channel::GetUnits()
{
	return m_units;
}

void Channel::AddValue( double value)
{
    m_values.push_back(value);

    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}

void Channel::ClearValues()
{
	m_values.clear();

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
    _DisplayNAValue();
}

bool Channel::IsOnHorizontalAxis()
{ return m_axis->IsHorizontal(); }


void Channel::_SetMinimumSize()
{
     setMinimumSize(GetMinimumSize());
}

void Channel::_ShowLastValueWithUnits()
{
    QString textWithSpace = m_lastValueText + " " + m_units;
    unsigned widthMax = m_valueLabel->GetLongestTextSize().width();
    unsigned widthSpace = m_valueLabel->GetSize(textWithSpace).width();
    m_valueLabel->setText(
        (widthMax >= widthSpace) ? textWithSpace : m_lastValueText + "<br/>" + m_units);
    _SetMinimumSize();
    m_axis->UpdateGraphAxisName();
}

void Channel::_DisplayNAValue()
{
    //m_lastValueText = "-0.000e-00<br/>mA";
    m_lastValueText = tr("n/a");
    _ShowLastValueWithUnits();
}

void Channel::_UpdateTitle()
{
    setTitle(
        QString("(%1) ").arg(m_hwIndex + 1) +
        (IsOnHorizontalAxis() ? "- " : "| ") +
        m_name
    );
    m_axis->UpdateGraphAxisName();
}

void Channel::mousePressEvent(QMouseEvent * event)
{
    ChannelSettings *settings = new ChannelSettings(this, m_context);
    settings->exec();
}

void Channel::displayValueOnIndex(int index)
{
    if (0 == index && 0 == m_values.size())
        return; //probably setRange in start method

    double value = m_values[index];
    double absValue = std::abs(value);

    QString strValue;
    if (absValue < 0.0001 && absValue != 0)
        strValue = QString::number(value, 'e', 3);
    else if (absValue < 1)
        strValue = QString::number(value, 'g', 4);
    else
        strValue = QString::number(value, 'g', 6);

    m_lastValueText = strValue;
    _ShowLastValueWithUnits();

    m_graphPoint->clearData();
    m_graphPoint->addData(index, m_values[index]);
}


QCPGraph *Channel::GetGraph()
{
    return m_graph;
}

QCPGraph *Channel::GetGraphPoint()
{
    return m_graphPoint;
}
void Channel::UpdateGraph(double xValue)
{
    m_graph->data()->insert(xValue, QCPData(xValue, m_values.last()));

}

void Channel::_ShowOrHideGraphAndPoin(bool shown)
{
    m_graph->setVisible(shown);
    m_graphPoint->setVisible(shown);
}

void Channel::AssignToGraphAxis(QCPAxis *graphAxis)
{
    if (m_axis->IsHorizontal())
        return;

    m_graph->setValueAxis(graphAxis);
    m_graphPoint->setValueAxis(graphAxis);
}

void Channel::AssignToAxis(Axis *axis)
{
    m_axis = axis;
    AssignToGraphAxis(axis->GetGraphAxis());
    m_axis->UpdateGraphAxisName();
    m_axis->UpdateVisiblility();
}

void Channel::setVisible(bool visible)
{
    QGroupBox::setVisible(visible);
    m_axis->UpdateGraphAxisName();
    m_axis->UpdateVisiblility();
}
