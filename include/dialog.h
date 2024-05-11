#ifndef DIALOG_H
#define DIALOG_H

#include <QMainWindow>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(int type,QWidget *parent = nullptr);
    QString admin();
    QString passwd();
    bool confirmPasswd();

private slots:

private:
    QPushButton *m_adminButton;
    QLineEdit *m_adminLineEdit;
    QPushButton *m_passwdButton;
    QLineEdit *m_passwdLineEdit;
    QPushButton *m_confirmButton;
    QLineEdit *m_confirmLineEdit;
    QDialogButtonBox *m_buttonBox;
};

#endif // DIALOG_H
