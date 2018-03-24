#ifndef CHANNELSETTINGS_H
#define CHANNELSETTINGS_H

#include <bases/FormDialogColor.h>
#include <QColor>
#include <QVector>
#include <memory>
#include <hw/Sensor.h>

namespace bases { class ComboBox; }
namespace hw { class SensorManager; class SensorQuantity; }
class Axis;
class ChannelBase;
class ChannelWidget;
class GraphicsContainer;
class Measurement;
class SampleChannel;
class QCheckBox;
class QFormLayout;
class QLineEdit;
class QString;

class ChannelSettings : public bases::FormDialogColor
{
    Q_OBJECT

    void _InitializeShapeCombo(ChannelWidget *channelWidget);
    void _InitializeAxisCombo();
    bool _MoveLastHorizontalToVertical();
    virtual bool BeforeAccept();
    void _InitializeTimeFeatures();
    void _RefillAxisCombo();
    bool _AxisCheckForRealTimeMode();
    void _InitializePenStyle(Qt::PenStyle selected);
    void _InitializeValueLine(ChannelWidget *channelWidget);
    void _InitializeGhostCombos();
    void _FillMeasurementCombo();
    void _InitializeSensorItems();
    void _InitializeSensorItem(bases::ComboBox **item, const QString &label, const char *slot);
    void _FillSensorQuanitityCB();
    void _FillSensorNameCB();
    void _FillSensorPortCB();
    QString _GetQuantityString(hw::SensorQuantity *quantity);
    QString _GetPortName(int port);

    QVector<Measurement *> m_measurements;
    GraphicsContainer *m_graphicsContainer;
    ChannelWidget *m_channelWidget;
    ChannelBase *m_channel;
    QLineEdit *m_currentValueControl;
    QLineEdit * m_name;
	QLineEdit * m_units;
    bases::ComboBox * m_measurementCombo;
    bases::ComboBox * m_channelCombo;
    bases::ComboBox * m_shapeComboBox;
    bases::ComboBox * m_axisComboBox;
    bases::ComboBox * m_style;
    bases::ComboBox * m_timeUnits;
    bases::ComboBox * m_format;
    bases::ComboBox * m_penStyle;
    bases::ComboBox * m_sensorQuantityComboBox;
    bases::ComboBox * m_sensorNameComboBox;
    bases::ComboBox * m_sensorPortComboBox;

    bool m_currentValueChanged;
    double m_currentValue;
    hw::SensorManager *m_sensorManager;

public:
    ChannelSettings(
        QVector<Measurement *> measurements,
        GraphicsContainer *graphicsContainer,
        ChannelWidget *channelWidget,
        hw::SensorManager *sensorManager);
    GraphicsContainer *GetGraphicsContainer();
signals:

private slots:
    void axisChanged(int index);
    void styleChanged(int index);
    void currentValueChanged(QString const &content);
    void setOriginalValue(bool checked);
    void setNaValue(bool);
    void fillChannelCombo(int measurementComboIndex);
    void loadFromOriginalWidget(int channelComboIndex);
    void sensorQualityChanged(int index);
    void sensorNameChanged(int index);
    void sensorPortChanged(int index);
};

#endif // CHANNELSETTINGS_H
