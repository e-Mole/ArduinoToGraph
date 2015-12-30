#include "AxisMenu.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <Context.h>
#include <Channel.h>
#include <Measurement.h>
#include <Plot.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPalette>
#include <QMap>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QWidget>

AxisMenu::AxisMenu(const Context &context,  Measurement &measurement) :
    MenuDialogBase(tr("Axes")),
    m_context(context),
    m_measurement(measurement)
{
    ReinitGrid();
}

void AxisMenu::_AddRowWithEditAndRemove(Axis *axis)
{
    QWidget *rowWidget = new QWidget(this);
    QHBoxLayout * buttonLayout = new QHBoxLayout(rowWidget);
    buttonLayout->setMargin(0);
    rowWidget->setLayout(buttonLayout);

    QPushButton * editButton = new QPushButton(tr("Edit"), rowWidget);
    buttonLayout->addWidget(editButton);
    m_editButtontoAxis.insert(editButton, axis);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()));

    QPushButton * removeButton = new QPushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(axis->IsRemovable());
    buttonLayout->addWidget(removeButton);
    m_removeButtontoAxis.insert(removeButton, axis);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()));

    QLabel *label = new QLabel(axis->GetTitle(), this);
    QPalette palette(label->palette());
    palette.setColor(QPalette::Foreground, axis->GetColor());
    label->setPalette(palette);
    m_formLayout->addRow(label, rowWidget);
}

void AxisMenu::FillGrid()
{
    foreach (Axis *axis, m_measurement.GetAxes())
        _AddRowWithEditAndRemove(axis);

    QPushButton * addbutton = new QPushButton(tr("Add"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
}

void AxisMenu::addButtonPressed()
{
    Axis *newAxis = m_measurement.CreateAxis(Qt::black);
    AxisSettings dialog(newAxis, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
        ReinitGrid();
    }
    else
        m_measurement.RemoveAxis(newAxis);

    CloseIfPopup();
}

void AxisMenu::removeButtonPressed()
{
    Axis *axis = m_removeButtontoAxis.find((QPushButton*)sender()).value();
    Axis *firstVertical = NULL;
    foreach (Axis * axis, m_measurement.GetAxes())
    {
        //first vertical is not possible to delete as same as horizontal
        if (!axis->IsHorizontal())
        {
            firstVertical = axis;
            break;
        }
    }

    foreach (Channel * channel, axis->GetMeasurement()->GetChannels())
    {
        if (axis == channel->GetAxis())
        {
            if (1 == //standardButton1 pressed (Cancel)
                QMessageBox::question(
                    this,
                    m_context.m_applicationName,
                    QString(tr("All channels assigned to the axis '%1' will be moved to an axis '%2'.")).
                        arg(axis->GetTitle()).arg(firstVertical->GetTitle()),
                    tr("Remove anyway"), tr("Cancel")
                )
            )
            {
                CloseIfPopup();
                return;
            }

        }
    }

    foreach (Channel * channel, axis->GetMeasurement()->GetChannels())
    {
        if (axis == channel->GetAxis())
            channel->SetAxis(firstVertical);
    }
    m_measurement.RemoveAxis(axis);
    firstVertical->UpdateGraphAxisName();
    firstVertical->UpdateVisiblility();

    ReinitGrid();
    adjustSize();
    CloseIfPopup();
}

void AxisMenu::editButtonPressed()
{
    Axis *axis = m_editButtontoAxis.find((QPushButton*)sender()).value();
    AxisSettings dialog(axis, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
        ReinitGrid();
    }

    close();
}