#ifndef CHANNELGRAPH_H
#define CHANNELGRAPH_H

#include <qcustomplot/qcustomplot.h>

class Axis;
class ChannelGraph : public QCPGraph
{
    bool m_showAllMarks;
    double m_selectedHorizontalValue;
    QCPScatterStyle m_selectedMarkStyle;
    Axis *m_valueAxis;
    bool m_active;

    void drawScatterPlot(QCPPainter *painter, QVector<QCPData> *scatterData) const;

    //to be hidden
    void setScatterStyle(const QCPScatterStyle &style)
    { QCPGraph::setScatterStyle(style); }
    QCPScatterStyle scatterStyle() const
    { return QCPGraph::scatterStyle(); }

public:
    ChannelGraph(
        QCPAxis *keyAxis,
        Axis *valueAxis,
        QColor const &color,
        unsigned shapeIndex,
        bool showAllMarks,
        Qt::PenStyle penStyle
    );

    void SetMarkShape(unsigned shapeIndex);
    void ChangeSelectedHorizontalValue(double horizontalValue);
    void SetAllMarksShown(bool showAllMarks);
    void SetColor(const QColor& color);
    void SetPenStyle(Qt::PenStyle penStyle);
    int GetShapeIndex() const;
    Axis *GetValuleAxis() const
    { return m_valueAxis;}
    void AssignToAxis(Axis *axis);
    void AssignToGraphAxis(QCPAxis *graphAxis);
    void SetActive(bool active);
    void SetPenWidth(double width);
    bool AreAllMarksShown() { return m_showAllMarks; }
};

#endif // CHANNELGRAPH_H
