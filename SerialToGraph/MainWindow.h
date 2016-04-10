#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
#include <hw/HwSink.h>
#include <QMainWindow>
#include <GlobalSettings.h>
#include <QString>
#include <QVector>

class CentralWidget;
class Console;
class Measurement;
class PortListDialog;
class QApplication;
class QDataStream;
class QTabWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

    void _SetCurrentFileName(QString const &fileName);
    void _WriteUnsupportedFileVersion();
    void keyReleaseEvent(QKeyEvent * event);

    GlobalSettings m_settings;
    hw::HwSink m_hwSink;
    ButtonLine* m_buttonLine;
    QVector<Measurement*> m_measurements;
    Context m_context;
    QTabWidget *m_measurementTabs;
    Measurement *m_currentMeasurement;
    QString m_currentFileName;
    QString m_currentFileNameWithPath;
    QString m_langBcp47;
    PortListDialog *m_portListDialog;
    Console *m_console;
public:
    MainWindow(QApplication const &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent = 0);
    ~MainWindow();

    Measurement *CreateNewMeasurement(bool initializeAxesandChannels);
    Measurement *CloneCurrentMeasurement();
    void ConfirmMeasurement(Measurement *m);
    void SwichCurrentMeasurement(Measurement *m);
    void RemoveAllMeasurements();
    void RemoveMeasurement(Measurement *m, bool confirmed);
    Measurement * GetCurrnetMeasurement();
    void DeserializeMeasurements(QString const &fileName, bool values);
    void SerializeMeasurements(const QString &fileName, bool values);
    QString &GetCurrentFileNameWithPath();
    void OpenNew();
    void RefreshHwConnection();
    void ShowConsole(bool show);
    void TerminateBluetooth();

private slots:
    void measurementNameChanged();
    void currentMeasurementChanged(int index);
    void measurementColorChanged();
    void consoleLocationChanged(Qt::DockWidgetArea area);
public slots:
    void openSerialPort();
};

#endif // MAINWINDOW_H
