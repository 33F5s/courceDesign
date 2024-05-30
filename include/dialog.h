#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

typedef enum {Login=0,Register,ChangePassword,DeleteUser}userAction;

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(userAction ua,QWidget *parent = nullptr,int permi=0);
    QString admin();
    QString passwd();
    int getPermission();
    void setAdmin(const QString &admin);
    void setNoEditedAdmin();

private slots:

private:
    userAction ua;
    int permission;
    QPushButton *m_adminButton;
    QLineEdit *m_adminLineEdit;
    QPushButton *m_passwdButton;
    QLineEdit *m_passwdLineEdit;
    QPushButton *m_confirmButton;
    QLineEdit *m_confirmLineEdit;
    QDialogButtonBox *m_buttonBox;

    void LoginSlot();
    void RegisterSlot();
    void ChangePasswordSlot();
    void DeleteUserSlot();
};

#endif // DIALOG_H
