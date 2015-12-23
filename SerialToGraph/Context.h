#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class Channel;
class Graph;
class Measurement;
class Plot;
class QSettings;

struct Context
{
public:
    Context(QVector<Axis*> &axis,
        QVector<Channel*> &channels,
        QVector<Measurement *> &measurements,
        QSettings &settings);

    QVector<Axis*> &m_axes;
    QVector<Channel*> &m_channels;
    QString m_applicationName;
    QSettings &m_settings;
    Graph *m_graph;
    Plot *m_plot;
    QVector<Measurement *> &m_measurements;
    Measurement *m_currentMeasurement;

    void SetGraph(Graph *graph, Plot *plot);
    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
