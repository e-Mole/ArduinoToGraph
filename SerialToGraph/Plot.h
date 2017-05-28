#ifndef PLOT_H
#define PLOT_H

#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QPointF>
#include <QTime>
#include <QPoint>
#include <QPair>
#include <QMap>

class QColor;
class QEvent;
class QGestureEvent;
class Context;
class Measurement;
class Axis;

class MyAxisRect : public QCPAxisRect
{
    Q_OBJECT

public:
    MyAxisRect(QCustomPlot *parentPlot, bool setupDefaultAxes=true) : QCPAxisRect(parentPlot, setupDefaultAxes)
    {}

    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    bool IsDragging(){ return mDragging; }
};

class Plot : public QCustomPlot
{
    Q_OBJECT

    friend class PlotContextMenu;
public:
    enum DisplayMode
    {
        SampleValue,
        DeltaValue,
        MaxValue,
        MinValue
    };

private:

    enum MarkerTypeSelection{
      MTSSample,
      MTSRangeAutoBorder,
      MTSRangeLeftBorder,
      MTSRangeRightBorder
    };

    void _SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis);
    bool _IsGraphAxisEmpty(QCPAxis *graphAxis);
    double _GetClosestXValue(double in);
    bool _GetClosestXIndex(double in, int &out);
    QCPItemLine *_AddMarkerLine(QCPItemLine *markerLine, int position, const QColor color);
    int _MinMaxCorection(int xIndex);
    QColor _SetMarkerLineColor(bool isSame, bool isCurrent);
    QCPItemRect *_DrawOutRect(bool isLeft, int position);

    Measurement &m_measurement;
    bool m_disabled;
    QPair<int, int> m_markerPositions;
    QPair<QCPItemLine *, QCPItemLine *> m_markerLines;
    QCPItemLine *m_selectedLine;
    QPair<QCPItemRect *, QCPItemRect *> m_outRect;
    bool m_mouseHandled;
    QTime m_clickTime;
    QMouseEvent *m_mouseMoveEvent;
    QWheelEvent *m_wheelEvent;
    QPoint m_mousePressPosition;
    QPoint m_mouseReleasePosition;
    DisplayMode m_displayMode;
    MarkerTypeSelection m_markerTypeSelection;
    ChannelBase::DisplayValue m_markerRangeValue;
    QList<ChannelGraph *> m_channelGraphs;

    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    bool event( QEvent *event );
protected:
    //I don't want to use virtual one, I want to decide alone in evane handling
    void MyMousePressEvent(QMouseEvent *event);
    void MyMouseReleaseEvent(QMouseEvent *event);
    void MyMouseMoveEvent(QMouseEvent *event);


public:
    Plot(Measurement *measurement);

    MyAxisRect *axisRect()
    { return (MyAxisRect*)QCustomPlot::axisRect(); }

    void SetDisabled(bool disable);
    void ReplotIfNotDisabled();
    void SetGraphColor(ChannelGraph *graph, const QColor &color);
    void SetPenStyle(ChannelGraph *graph, Qt::PenStyle penStyle);
    unsigned GetShape(ChannelGraph *graph);
    void RemoveAxis(QCPAxis *axis);
    QCPAxis *AddYAxis(bool onRight);
    void RescaleAxis(QCPAxis *axis);
    void RescaleAllAxes();
    void PauseDrawing();
    void ContinueDrawing();
    void SetDrawingInProcess(bool set);
    void WaitForDrawingIsFinished();
    void RemoveGraph(ChannelGraph *graph);
    void RefillGraphs();
    void SetAxisStyle(QCPAxis *axis, bool dateTime, QString const &format);
    void SetMarkerLine(int position);
    void Zoom(const QPointF &pos, int delta);
    void ZoomToFit();
    DisplayMode GetDisplayMode() { return m_displayMode; }
    void SetDisplayMode(DisplayMode mode) { m_displayMode = mode; }
    void DisplayChannelValue(ChannelBase *channel);
    bool IsInRangeMode() { return m_markerTypeSelection != MTSSample; }

    ChannelGraph *AddChannelGraph(QCPAxis *keyAxis, Axis *valueAxis, const QColor &color,
        unsigned shapeIndex,
        bool shapeVisible,
        Qt::PenStyle lineStyle);
signals:
    void markerLinePositionChanged(int xIndex);
public slots:
private slots:
    void selectionChanged();
    void procesMouseMoveEvent();
    void processWheelEvent();

};

#endif // PLOT_H
