#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class ChannelBase;
class GlobalSettings;
class MainWindow;
class Measurement;
class Plot;

struct Context
{
public:
    Context(QVector<Measurement *> &measurements,
        GlobalSettings &settings,
        MainWindow &mainWindow);

    GlobalSettings &m_settings;
    QVector<Measurement *> &m_measurements;
    MainWindow &m_mainWindow;

    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
