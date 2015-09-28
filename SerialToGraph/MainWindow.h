#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT

    QSettings m_settings;
	SerialPort m_serialPort;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
protected slots:
    void buttonLineLocationChanged(Qt::DockWidgetArea area);
	void channelSideBaeLocationChanged(Qt::DockWidgetArea area);
    void dockVisibilityChanged(bool visible);
};

#endif // MAINWINDOW_H
