#ifndef HWCHANNELPROPERTIES_H
#define HWCHANNELPROPERTIES_H

#include <graphics/ChannelProxyBase.h>

class ChannelBase;
class ChannelWidget;
class HwChannel;
class QObject;
namespace hw {class Sensor; class SensorQuantity; }

class HwChannelProxy : public ChannelProxyBase
{
    HwChannel *_GetChannel() const;
public:
    HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);
    virtual double GetValue(unsigned index) const;
    void ChangeValue(int index, double newValue);
    virtual HwChannelProxy *Clone(QObject *parent, ChannelWidget *newPrxoy);
    bool IsActive();
    hw::Sensor *GetSensor();
    void SetSensor(hw::Sensor *sensor);
    unsigned GetSensorPort();
    void SetSensorPort(unsigned sensorPort);
    hw::SensorQuantity *GetSensorQuantity();
    void SetSensorQuantity(hw::SensorQuantity *sensorQuantity, unsigned order);
    double GetOriginalValue(int index);
};

#endif // HWCHANNELPROPERTIES_H
