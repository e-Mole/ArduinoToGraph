#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QColor>
#include <bases/Label.h>
#include <QObject>

class Measurement;
class QString;

class ChannelBase : public QObject
{
    friend class ChannelSettings;
    Q_OBJECT

    Q_ENUMS(Qt::PenStyle)

public:
    enum ValueType
    {
        ValueTypeUnknown,
        ValueTypeSample,
        ValueTypeOriginal,
        ValueTypeChanged,
        ValueTypeRangeValue
    };

private:
    void _SetName(QString name);
    QString _GetName();
protected:
    void mousePressEvent(QMouseEvent * event);
    void _UpdateExtremes(double value, unsigned index);
    void _RecalculateExtremes();

    Measurement * m_measurement;
    QVector<double> m_values;
    double m_channelMinValue;
    double m_channelMaxValue;
    unsigned m_channelMinValueIndex;
    unsigned m_channelMaxValueIndex;


public:
    enum Type
    {
        Type_Sample,
        Type_Hw
    };

    ChannelBase(Measurement *measurement);

    virtual Type GetType() = 0;

    virtual unsigned GetValueCount() const
    { return m_values.size();}

    double GetRawValue(int index) const;

    virtual void AddValue( double value);

    double GetMinValue()
    { return m_channelMinValue; }
    double GetMaxValue()
    { return m_channelMaxValue; }

    unsigned GetMinValueIndex()
    { return m_channelMinValueIndex; }
    unsigned GetMaxValueIndex()
    { return m_channelMaxValueIndex; }

    Measurement * GetMeasurement() const;

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}

    virtual ValueType GetValueType(int index) { Q_UNUSED(index); return ValueTypeUnknown; }
    static double GetNaValue();
    static bool IsEqual(double first, double second);
    virtual double GetValueWithCorrection(int index) {return m_values[index]; }
};

#endif // CHANNEL_H
