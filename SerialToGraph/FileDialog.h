#ifndef FILEDIALOG_H
#define FILEDIALOG_H

class QString;
class QWidget;
class FileDialog
{
public:
    static QString getOpenFileName(QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter,
        const QString &limit
    );

    static QString getSaveFileName(QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &filter,
        const QString &limit
    );

    static QString getExistingDirectory(
        QWidget *parent,
        const QString &caption,
        const QString &dir,
        const QString &limit
    );
};

#endif // FILEDIALOG_H
