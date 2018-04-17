#ifndef GRAPHICSCONTAINER_H
#define GRAPHICSCONTAINER_H

#include <ChannelBase.h>
#include <graphics/ChannelProperties.h>
#include <SampleChannel.h>
#include <QMap>
#include <QString>
#include <QVector>
#include <QWidget>
#include <set>
#include <map>

class Axis;
class Color;
class ChannelGraph;
class ChannelProxyBase;
class ChannelWidget;
class HwChannel;
class HwChannelProxy;
class KeyShortcut;
class Measurement;
class Plot;
class QColor;
class QCPAxis;
class QHBoxLayout;
class QGridLayout;
class QScrollBar;
class QVBoxLayout;

class GraphicsContainer : public QWidget
{
    Q_OBJECT
    Measurement *m_mainMeasurement;
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_plotAndSliderLayout;
    QVBoxLayout *m_displaysAndSliderLayout;
    QGridLayout *m_displayLayout;
    bool m_grid;
    QString m_name;
    Plot *m_plot;
    QScrollBar *m_scrollBar;
    unsigned m_currentIndex;
    bool m_followMode;
    QVector<ChannelProxyBase *> m_channelProxies;
    std::set<double> m_horizontalValueSet;
    double m_lastMeasuredHorizontalValue;
    QVector<Axis*> m_axes;

    bool m_marksShown;
    ChannelWidget *m_sampleChannelWidget;
    SampleChannelProxy *m_sampleChannelProxy;
    KeyShortcut *m_plotKeyShortcut;

    KeyShortcut *m_allChannelsShortcut;
    KeyShortcut *m_noChannelsShortcut;
    QMap<Measurement*, ChannelProxyBase*> m_horizontalChannelMapping;
    ChannelProxyBase *m_ghostWaitingForConfirmation;

    virtual void resizeEvent(QResizeEvent *){ resized(); }
    virtual QSize sizeHint() const { return QSize(800,700); }

    void _InitializeLayouts();
    void _FollowLastMeasuredValue();
    QCPAxis * _GetGraphAxis(unsigned index);
    Axis * _CreateAxis(QColor const & color, QCPAxis *graphAxis);
    void _CreateKeyShortcuts();
    void _RemoveKeyShortcuts();
    void _DisplayChannelValue(ChannelProxyBase *channelProxy);
    bool _IsTracked(Measurement *m);
    SampleChannelProxy *_CreateSampleChannelProxy(SampleChannel *channel, ChannelWidget *widget, SampleChannelProperties *properties, bool isGhost);
    HwChannelProxy *_CreateHwCannelProxy(HwChannel *channel, ChannelWidget *widget, ChannelProperties *properties, bool isGhost);

public:
    GraphicsContainer(QWidget *parent, Measurement *mainMeasurement, QString const &name, bool markShown);
    ~GraphicsContainer();
    void SetGrid(bool grid);
    void RemoveChannelProxy(ChannelProxyBase *channelProxy);
    QString &GetName() {return m_name; }
    void SetName(QString const &name) {m_name = name; } //TODO:signal?
    Plot *GetPlot() const;
    void SetFollowMode(bool set);
    void RedrawChannelValues();
    int GetCurrentIndex() { return m_currentIndex; }
    void AddHorizontalValue(double value);
    void ClearHorizontalValueSet();
    void CalculateScrollbarRange();
    void ReadingValuesPostProcess(double lastHorizontalValue);
    void ShowGraph(bool show);
    bool IsPlotVisible() const;
    bool IsPlotInRangeMode();
    unsigned GetClosestHorizontalValueIndex(double value) const;
    double GetHorizontalValueBySliderPos(unsigned position) const;
    int GetSliderPos();
    double GetLastMeasuredHorizontalValue(Measurement *m);
    void SetHorizontalChannel(Measurement *m, ChannelBase *channel, ChannelProxyBase *originalHorizontalChannelProxy);
    void SetHorizontalChannel(Measurement *m, ChannelBase *channel);
    Axis * CreateNewAxis(unsigned index);
    Axis * CreateYAxis(QColor const & color);
    void RemoveAxis(Axis * axis);
    QVector<Axis *> const & GetAxes() const;
    Axis *GetAxis(int index);
    unsigned GetAxisCount();
    int GetAxisIndex(Axis *axis);
    Axis * GetFirstVerticalAxis();
    void CloneAxes(QVector<Axis *> const &sourceAxes);
    bool GetMarksShown();
    void SetMarksShown(bool marksShown);
    ChannelGraph *AddChannelGraph(Axis *valueAxis);
    Axis *InitializeHorizontalAxis();
    Axis *InitializeVerticalAxis();
    void UpdateAxisNames();
    void UpdateAxis(Axis *axis);
    void UpdateAxes();
    ChannelGraph * AddBlackChannelGraph(Axis *valueAxis);
    ChannelGraph * AddChannelGraph(Axis *valueAxis, QColor const &color, unsigned shapeIndex, Qt::PenStyle penStyle);
    void SetAxisStyle(Axis *axis, bool dateTime, QString const &format);
    void RescaleAxes(ChannelWidget *channelWidget);
    ChannelProxyBase *GetHorizontalChannelProxy() const;
    ChannelProxyBase *GetHorizontalChannelProxy(Measurement *measurement) const;
    QVector<ChannelProxyBase *> &GetChannelProxies();
    unsigned GetChannelProxiesCount();
    ChannelWidget *GetChannelWidget(ChannelBase *channel);
    ChannelWidget *GetChannelWidget(unsigned index);
    SampleChannelProxy *GetSampleChannelProxy();
    bool IsHorizontalValueSetEmpty();
    void RecalculateSliderMaximum();
    ChannelGraph* CloneChannelGraph(GraphicsContainer *sourceContainer, ChannelWidget *sourceChannelWidget);
    QColor GetColorByOrder(unsigned order);
    SampleChannelProxy *CreateSampleChannelProxy(SampleChannel *channel, Axis *valueAxis, bool isGhost);
    SampleChannelProxy *CloneSampleChannelProxy(SampleChannelProxy *sourceChannelProxy, SampleChannel *channel, bool isGhost);
    void GhostAddingChangingPostProcess(ChannelProxyBase *ghostProxy);

    HwChannelProxy *CreateHwChannelProxy(HwChannel *channel,
        Axis *valueAxis,
        unsigned shortcutOrder,
        QString const name,
        QColor const &color,
        bool visible,
        QString const & units,
        bool isGhost);

    HwChannelProxy *CloneHwChannelProxy(HwChannelProxy *sourceChannelProxy, HwChannel *channel, bool isGhost);
    QString GetValueTimestamp(SampleChannelProxy *channelProxy, unsigned index);
    void Activate();
    void Deactivate();
    QKeySequence GetPlotKeySequence();
    QKeySequence GetChannelKeySequence(ChannelProxyBase *channelProxy);
    QKeySequence GetAllChannelsSequence();
    QKeySequence GetNoChannelsSequence();
    void ActivateChannel(ChannelProxyBase *channelProxy, bool checked);
    void RefillWidgets();
    ChannelProxyBase *AddGhost(ChannelProxyBase *sourceChannelProxy,
        GraphicsContainer *sourceGraphicsContainer,
        ChannelBase *sourceHorizontalChannel
    , bool confirmed);

    static QString GetGhostName(GraphicsContainer * sourceGraphicsContainer, ChannelProxyBase *channelProxy);
    void ConfirmGhostChannel();
    void RejectGhostChannel();
    ChannelProxyBase *GetChannelProxy(ChannelBase *channel);
    ChannelProxyBase *GetChannelProxy(ChannelWidget *widget);
    ChannelProxyBase *GetChannelProxy(ChannelProperties *properties);
    int GetLastHorizontalValueIndex(Measurement *m, unsigned markerPosition);
    void ReplaceChannelProxy(ChannelProxyBase *oldProxy, ChannelProxyBase *newProxy);
signals:
    void resized();
    void editChannel(ChannelProxyBase *channelProxy);

public slots:
    void sliderValueChanged(int value);
    void editChannel();
    void addNewValueSet();
    void sampleChannelPropertyChanged();
    void activateChannelKeyShortcut();
    void plotKeyShortcut();
    void noChannelsKeyShortcut();
    void allChannelsKeyShortcut();
    void replaceDisplays();
private slots:
    void markerLinePositionChanged(int position);
    void hwValueChanged(unsigned index);
};

#endif // GRAPHICSCONTAINER_H
