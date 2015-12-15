#include "AxisEditDialog.h"
#include <Axis.h>
#include <Context.h>
#include <MyCustomPlot.h>
#include <QColorDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QVector>
#include <QWidget>

AxisEditDialog::AxisEditDialog(Axis *axis, Context const & context) :
    FormDialogBase(NULL, tr("Edit Axis...")),
    m_axisOriginal(axis),
    m_axisCopy(*axis),
    m_name(NULL),
    m_colorButtonWidget(NULL),
    m_context(context)
{
    m_formLayout->addRow(new QLabel(tr("Type"), this), new QLabel(axis->IsHorizontal() ? tr("Horizontal") : tr("Vertical"), this));

    m_name = new QLineEdit(axis->GetTitle(), this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);
    connect(m_name, SIGNAL(textChanged(QString)), this, SLOT(nameChanged(QString)));

    QPushButton * colorButton = new QPushButton("", this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(7);
    m_colorButtonWidget = new QWidget(colorButton);
    _SetColorButtonColor(axis->GetColor());
    layout->addWidget(m_colorButtonWidget);
    colorButton->setLayout(layout);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));

    m_formLayout->addRow(new QLabel(tr("Color"), this), colorButton);

    if (!axis->m_isHorizontal)
    {
        QComboBox *side = new QComboBox(this);
        side->addItem(tr("Left"));
        side->addItem(tr("Right"));
        side->setCurrentIndex((int)m_axisCopy.m_isOnRight);
        connect(side, SIGNAL(currentIndexChanged(int)), this, SLOT(sideChanged(int)));
        m_formLayout->addRow(new QLabel(tr("Side"), this), side);
    }

    QComboBox *display = new QComboBox(this);
    display->addItem(tr("Channels and Units"));
    display->addItem(tr("Name"));
    display->setCurrentIndex((int)m_axisCopy.m_displayName);
    connect(display, SIGNAL(currentIndexChanged(int)), this, SLOT(displayChanged(int)));
    m_formLayout->addRow(new QLabel(tr("Display in Graph"), this), display);

}

void AxisEditDialog::nameChanged(QString const &text)
{
    m_axisCopy.m_title = text;
}

void AxisEditDialog::sideChanged(int index)
{
   m_axisCopy.m_isOnRight = (bool)index;
}

void AxisEditDialog::displayChanged(int index)
{
    m_axisCopy.m_displayName = (bool)index;
}

void AxisEditDialog::_SetColorButtonColor(QColor const &color)
{
    QString style = "background-color: rgb(%1, %2, %3);";
    m_colorButtonWidget->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));
}

void AxisEditDialog::BeforeAccept()
{

    bool sideChanged = m_axisOriginal->IsOnRight() != m_axisCopy.IsOnRight();
    bool displayNameChanged =
            m_axisOriginal->m_displayName != m_axisCopy.m_displayName ||
            (m_axisCopy.m_displayName && m_axisOriginal->m_title != m_axisCopy.m_title);

    *m_axisOriginal = m_axisCopy;

    //also set color to plot axis
    m_axisOriginal->_SetColor(m_axisCopy.m_color);
    
    if (!m_context.m_axes.contains(m_axisOriginal))
    {
        m_context.m_axes.push_back(m_axisOriginal);
        m_axisOriginal->_AssignGraphAxis(m_context.m_plot->AddYAxis(m_axisOriginal->IsOnRight()));
    }
    else
    {
        if (sideChanged)
            m_axisOriginal->_AssignGraphAxis(m_context.m_plot->AddYAxis(m_axisOriginal->IsOnRight()));
        if (displayNameChanged)
            m_axisOriginal->UpdateGraphAxisName();
    }


}

void AxisEditDialog::colorButtonClicked()
{
    QColorDialog colorDialog(NULL);
    colorDialog.setCurrentColor(m_axisCopy.m_color);
    colorDialog.setOption(QColorDialog::DontUseNativeDialog, true);
    if (QDialog::Accepted == colorDialog.exec())
    {
        m_axisCopy.m_color = colorDialog.currentColor();
        _SetColorButtonColor(m_axisCopy.m_color);
    }
}

