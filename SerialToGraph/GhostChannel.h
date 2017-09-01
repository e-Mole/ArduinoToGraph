#ifndef GHOSTCHANNEL_H
#define GHOSTCHANNEL_H

#include <ChannelBase.h>

class GhostChannel : public ChannelBase
{
    Q_OBJECT

    friend class ChannelSettings;

    ChannelBase *m_sourceChannel;

public:
    GhostChannel(ChannelBase * sourceChannel,
        Measurement *measurement,
        Context const & context,
        ChannelGraph *graph,
        QColor const &color = Qt::black,
        bool active = true,
        const QString &units = "",
        Qt::PenStyle penStyle = Qt::SolidLine);

    virtual Type GetType() { return Type_Ghost; }
    virtual unsigned GetShortcutOrder() { return ~0; }
    ChannelBase *GetSourceChannel() {return m_sourceChannel; }
    QString GenerateName(ChannelBase *sourceChannel);

    virtual unsigned GetValueCount() const
    { return m_sourceChannel->GetValueCount();}

    virtual double GetValue(unsigned index) const
    { return m_sourceChannel->GetValue(index); }

    virtual double GetLastValue()
    { return m_sourceChannel->GetLastValue(); }

    virtual double GetMinValue()
    { return m_sourceChannel->GetMinValue(); }

    virtual double GetMaxValue()
    { return m_sourceChannel->GetMaxValue(); }

    void FillGraph();
    virtual ValueType GetValueType(unsigned index) { Q_UNUSED(index); return ValueTypeUnknown; }
signals:

public slots:
};

#endif // GHOSTCHANNEL_H
