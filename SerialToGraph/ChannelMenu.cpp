#include "ChannelMenu.h"
#include <bases/ClickableLabel.h>
#include <ChannelWidget.h>
#include <ColorCheckBox.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <KeyShortcut.h>
#include <QKeySequence>
#include <MainWindow.h>
#include <Measurement.h>
#include <Plot.h>
#include <QGridLayout>
#include <bases/Label.h>
#include <QPalette>
#include <bases/PushButton.h>
#include <QSizePolicy>
#include <QShortcut>

ChannelMenu::ChannelMenu(GraphicsContainer *graphicsContainer, bool isGhostAddable) :
    bases::MenuDialogBase(graphicsContainer, tr("Panels")),
    m_graphicsContainer(graphicsContainer),
    m_plotShortcut(NULL),
    m_allChannelsShortcut(NULL),
    m_noChannelsShortcut(NULL),
    m_isGhostAddable(isGhostAddable)
{
#if !defined(Q_OS_ANDROID)
    CreatePanelShortcuts();
#endif
}

ChannelMenu::~ChannelMenu()
{
    delete m_plotShortcut;
    delete m_noChannelsShortcut;
    delete m_allChannelsShortcut;
    foreach (KeyShortcut *ks, m_shortcutChannels.keys())
        delete ks;
}

Label* ChannelMenu::_GetShortcutLabel(QString const &shortcut)
{
    return new Label(shortcut + " ", this);
}

void ChannelMenu::FillGrid()
{
    int row = 0;

    m_graphCheckBox = new ColorCheckBox(tr("Graph"), this);
    m_graphCheckBox->SetChecked(m_graphicsContainer->IsPlotVisible());
    connect(m_graphCheckBox, SIGNAL(clicked()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(m_graphCheckBox, row, 0);

    if (m_plotShortcut)
    {
        _AddShortcut(row, m_plotShortcut->GetText());
    }

    ++row;
    PushButton *showAllButton = new PushButton(tr("All Channels"), this);
    connect(showAllButton, SIGNAL(clicked()), this, SLOT(allChannelsActivated()));
    m_gridLayout->addWidget(showAllButton, row, 0, 1, 2);
    if (m_allChannelsShortcut)
    {
        _AddShortcut(row, m_allChannelsShortcut->GetText());
    }
    ++row;
    PushButton *showNoneButton = new PushButton(tr("No Channels"), this);
    connect(showNoneButton, SIGNAL(clicked()), this, SLOT(noChannelsActivated()));
    m_gridLayout->addWidget(showNoneButton, row, 0, 1, 2);
    if (m_noChannelsShortcut)
    {
        _AddShortcut(row, m_noChannelsShortcut->GetText());
    }
    ++row;
    PushButton *addGhostChannel = new PushButton(tr("Add Virtual Channel"), this);
    addGhostChannel->setEnabled(m_isGhostAddable);
    connect(addGhostChannel, SIGNAL(clicked()), this, SIGNAL(addGhostChannelActivated()));
    m_gridLayout->addWidget(addGhostChannel, row, 0, 1, 2);
    addGhostChannel->resize(addGhostChannel->sizeHint().width(), addGhostChannel->sizeHint().height());

    foreach (ChannelProxyBase *channelProxy, m_graphicsContainer->GetChannelProxies())
        _AddChannel(channelProxy);
}

void ChannelMenu::_AddShortcut(int row, QString const &shortcut)
{
    if (!shortcut.isEmpty())
        m_gridLayout->addWidget(_GetShortcutLabel(shortcut), row, 3);
}

void ChannelMenu::_AddChannel(ChannelProxyBase *channelProxy)
{
    unsigned rowNr = m_gridLayout->rowCount();
    ColorCheckBox *cb = new ColorCheckBox(channelProxy->GetName(), this);
    cb->SetChecked(channelProxy->isVisible());
    cb->SetColor(channelProxy->GetForeColor());

    m_channelCheckBoxes[channelProxy] = cb;
    m_checkBoxChannels[cb] = channelProxy;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivatedCheckBox()));
    m_gridLayout->addWidget(cb, rowNr, 0);

    _AddShortcut(rowNr, _GetChannelShortcutText(channelProxy));

    PushButton *editButton = new PushButton(tr("Edit"), this);
    m_editChannels[editButton] = channelProxy;
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
    m_gridLayout->addWidget(editButton, rowNr, 1);

    if (channelProxy->IsGhost())
    {
        PushButton *removeButton = new PushButton(tr("Remove"), this);
        m_editChannels[removeButton] = channelProxy;
        connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
        m_gridLayout->addWidget(removeButton, rowNr, 2);
        m_removeButtonToChannel.insert(removeButton, channelProxy);
    }
}

void ChannelMenu::UpdateCheckBoxes()
{
    for (auto it =  m_channelCheckBoxes.begin(); it != m_channelCheckBoxes.end(); ++it)
    {
        it.value()->SetText(it.key()->GetName());
        it.value()->SetColor(it.key()->GetForeColor());
    }
}
void ChannelMenu::edit()
{
    ChannelProxyBase *channelProxy = m_editChannels[(PushButton*)sender()];
    channelProxy->GetWidget()->clicked();

    ColorCheckBox *cb = m_channelCheckBoxes[channelProxy];
    cb->SetText(channelProxy->GetName());
    cb->SetColor(channelProxy->GetForeColor());
}

void ChannelMenu::remove()
{
    ChannelProxyBase * channelProxy = m_removeButtonToChannel[(PushButton*)sender()];
    m_channelCheckBoxes.remove(channelProxy);
    m_graphicsContainer->RemoveChannelProxy(channelProxy);


    for (int row = 0; row < m_gridLayout->rowCount(); row++)
    {
        for (int col = 0; col < m_gridLayout->columnCount(); col++)
        {
            QLayoutItem *item = m_gridLayout->itemAtPosition(row, col);
            m_gridLayout->removeItem(item);
            if (item != nullptr)
                delete item->widget();
            delete item;
        }
    }
    FillGrid();

    m_graphicsContainer->RecalculateSliderMaximum();
    adjustSize();
}

void ChannelMenu::ActivateChannel(ChannelProxyBase *channelProxy, bool checked)
{
    m_channelCheckBoxes[channelProxy]->SetChecked(checked);
    m_graphicsContainer->ActivateChannel(channelProxy, checked);
}

void ChannelMenu::graphActivated()
{
    //because of calling by shortcut
    m_graphCheckBox->SetChecked(!m_graphicsContainer->IsPlotVisible());
    m_graphicsContainer->plotKeyShortcut();
}

void ChannelMenu::noChannelsActivated()
{
    foreach (ChannelProxyBase *channelProxy, m_graphicsContainer->GetChannelProxies())
    {
        if (channelProxy->isVisible())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ActivateChannel(channelProxy, false);
        }
    }
}

void ChannelMenu::allChannelsActivated()
{
    foreach (ChannelProxyBase *channelProxy, m_graphicsContainer->GetChannelProxies())
    {
        if (!channelProxy->isVisible())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ActivateChannel(channelProxy, true);
        }
    }
}


QString ChannelMenu::_GetChannelShortcutText(ChannelProxyBase *channelProxy)
{
    //only a few channels to create inverted map
    for (auto it =  m_shortcutChannels.begin(); it != m_shortcutChannels.end(); ++it)
    {
        if (it.value() == channelProxy)
            return it.key()->GetText();
    }

    //will be reached on Android;
    return "";
}

void ChannelMenu::CreatePanelShortcuts()
{
    m_plotShortcut = new KeyShortcut(
        m_graphicsContainer->GetPlotKeySequence(), this, SLOT(graphActivated()));

    foreach (ChannelProxyBase *channelProxy, m_graphicsContainer->GetChannelProxies())
    {
        KeyShortcut *s = new KeyShortcut(
            m_graphicsContainer->GetChannelKeySequence(channelProxy),
            this,
            SLOT(channelActivatedShortcut())
        );
        if (s != NULL)
            m_shortcutChannels[s] = channelProxy;
    }

    m_allChannelsShortcut = new KeyShortcut(
        m_graphicsContainer->GetAllChannelsSequence(), this, SLOT(allChannelsActivated()));

    m_noChannelsShortcut = new KeyShortcut(
        m_graphicsContainer->GetNoChannelsSequence(), this, SLOT(noChannelsActivated()));
}

void ChannelMenu::channelActivatedCheckBox()
{
    ChannelProxyBase * channelProxy = m_checkBoxChannels[(ColorCheckBox*)sender()];
    ActivateChannel(channelProxy, !channelProxy->isVisible());
}

void ChannelMenu::channelActivatedShortcut()
{
    ChannelProxyBase *channelProxy = m_shortcutChannels[(KeyShortcut*)sender()];
    ActivateChannel(channelProxy, !channelProxy->isVisible());
}
