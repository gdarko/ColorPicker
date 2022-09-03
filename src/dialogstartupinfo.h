#ifndef DIALOGSTARTUPINFO_H
#define DIALOGSTARTUPINFO_H

#include <QDialog>

namespace Ui {
class DialogStartupInfo;
}

class DialogStartupInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStartupInfo(QWidget *parent = nullptr);
    ~DialogStartupInfo();
    void setDialogTitle(QString title);
    void setDialogDescription(QString description);

private:
    Ui::DialogStartupInfo *ui;
};

#endif // DIALOGSTARTUPINFO_H
