#ifndef AXISMENU_H
#define AXISMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>

class Axis;
class GraphicsContainer;
class Plot;
class PushButton;
class QWidget;
class AxisMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void FillGrid();
    void _AddRowWithEditAndRemove(Axis *axis);

    GraphicsContainer * m_graphicsContainer;
    QMap<PushButton*, Axis*> m_removeButtontoAxis;
    QMap<PushButton*, Axis*> m_editButtontoAxis;
public:
    AxisMenu(QWidget *parent, GraphicsContainer *graphicsContainer);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // AXISMENU_H
