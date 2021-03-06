#include "ChannelBase.h"
#include <cmath>
#include <GlobalSettings.h>
#include <Measurement.h>
#include <Plot.h>
#include <QBoxLayout>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDebug>
#include <QHBoxLayout>
#include <bases/Label.h>
#include <QLocale>
#include <QPalette>
#include <QPen>
#include <QString>
#include <limits>

ChannelBase::ChannelBase(Measurement *measurement):
    QObject(measurement),
    m_measurement(measurement),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_channelMinValueIndex(~0),
    m_channelMaxValueIndex(~0)
{
}

void ChannelBase::_UpdateExtremes(double value, unsigned index)
{
    if (value < m_channelMinValue && !IsEqual(value, -std::numeric_limits<double>::infinity()))
    {
        m_channelMinValue = value;
        m_channelMinValueIndex = index;
    }

    if (value > m_channelMaxValue && !IsEqual(value, -std::numeric_limits<double>::infinity()))
    {
        m_channelMaxValue = value;
        m_channelMaxValueIndex = index;
    }
}

void ChannelBase::AddValue( double value)
{
    unsigned index = m_values.count();
    m_values.push_back(value);

    if (value == GetNaValue())
        return;
}

Measurement * ChannelBase::GetMeasurement() const
{
    return m_measurement;
}

double ChannelBase::GetRawValue(int index) const
{
    if (index < 0 || index >= m_values.count())
    {
        return GetNaValue();
    }
    return m_values[index];
}

void ChannelBase::_RecalculateExtremes()
{
    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();

    for (unsigned i = 0; i < GetValueCount(); i++)
    {
        _UpdateExtremes(GetValueWithCorrection(i), i);
    }
}

double ChannelBase::GetNaValue()
{
    return std::numeric_limits<double>::infinity();
}

bool ChannelBase::IsEqual(double first, double second){
    if (qIsInf(first) && qIsInf(second))
        return true;
    if (qIsNull(first) && qIsNull(second))
        return  true;
    return qFuzzyCompare(first, second);
}
