#ifndef SETTINGDIALOGBASE_H
#define SETTINGDIALOGBASE_H

#include <bases/PlatformDialog.h>

class GlobalSettings;
class QFormLayout;
class QFrame;
class QString;

namespace bases
{
class FormDialogBase : public PlatformDialog
{
    Q_OBJECT
protected:
    virtual bool BeforeAccept() = 0;
    virtual void BeforeReject() {};
    virtual void closeEvent(QCloseEvent * e);
    QFormLayout *m_formLayout;
    bool m_acceptChangesByDialogClosing;
    bool m_changed;
public:
    FormDialogBase(QWidget *parent, const QString &title, bool acceptChangesByDialogClosing);

    bool IsChanged()
    {   return m_changed; }

    QFrame *AddSeparator();

private slots:
    void storeAndAccept();
};

} //namespace bases

#endif // SETTINGDIALOGBASE_H
