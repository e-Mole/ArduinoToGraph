#ifndef ADDDIRDIALOG_H
#define ADDDIRDIALOG_H

#include <bases/FormDialogBase.h>

class QLineEdit;

namespace file
{

class AddDirDialog : public bases::FormDialogBase
{
    Q_OBJECT

    bool BeforeAccept(){ return true; }

    QLineEdit *m_dirName;

public:
    AddDirDialog(QWidget *parent);
    QString GetDirName();
signals:

public slots:
};

} //namespace file

#endif // ADDDIRDIALOG_H
