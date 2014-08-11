#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H
#include "mngrquerys.h"

#include <QWidget>
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class FormSettings;
}

class FormSettings : public QDialog
{
    Q_OBJECT
private:
    Ui::FormSettings *ui;
    bool restoreDefault;
public:
    explicit FormSettings(QWidget *parent = 0);
    ~FormSettings();

    Sort::sort getSort();
    bool getSwitchToDir();
    bool getRestoreDefault();

//    bool ImportAppend();

private slots:
    void on_BtnBox_accepted();
    void BtnBox_resetDefaults();
    void on_BtnBox_clicked(QAbstractButton *button);
    void on_PBtn_Export_clicked();
    void on_TBtn_ChooseDir_clicked();
    void on_PBtn_ImAppend_clicked();
};

#endif // FORMSETTINGS_H
