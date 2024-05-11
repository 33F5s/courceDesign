#include "../include/dialog.h"

Dialog::Dialog(int type=0, QWidget *parent)
    : QDialog(parent)
{

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    if(type==0){
        setFixedSize(280, 180);
        setWindowTitle(QString::fromLocal8Bit("登录"));

        m_buttonBox->setFixedSize(180, 40);
        m_buttonBox->move(30, 120);
    }
    else{
        setFixedSize(280, 220);

        setWindowTitle(QString::fromLocal8Bit("注册"));
        m_confirmButton = new QPushButton(tr("确认密码"), this);
        m_confirmButton->setFixedSize(80, 30);
        m_confirmButton->move(30, 110);

        m_confirmLineEdit = new QLineEdit(this);
        m_confirmLineEdit->setAlignment(Qt::AlignHCenter);
        m_confirmLineEdit->setEchoMode(QLineEdit::Password);
        m_confirmLineEdit->setFixedSize(140, 30);
        m_confirmLineEdit->move(130, 110);

        m_buttonBox->setFixedSize(180, 40);
        m_buttonBox->move(30, 160);
    }

    m_adminButton = new QPushButton(tr("用户名"), this);
    m_adminButton->setFixedSize(80, 30);
    m_adminButton->move(30, 30);

    m_adminLineEdit = new QLineEdit(this);
    m_adminLineEdit->setAlignment(Qt::AlignHCenter);
    m_adminLineEdit->setFixedSize(140, 30);
    m_adminLineEdit->move(130, 30);

    m_passwdButton = new QPushButton(tr("密码"), this);
    m_passwdButton->setFixedSize(80, 30);
    m_passwdButton->move(30, 70);

    m_passwdLineEdit = new QLineEdit(this);
    m_passwdLineEdit->setAlignment(Qt::AlignHCenter);
    m_passwdLineEdit->setEchoMode(QLineEdit::Password);
    m_passwdLineEdit->setFixedSize(140, 30);
    m_passwdLineEdit->move(130, 70);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &Dialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &Dialog::reject);

}

QString Dialog::admin()
{
    return m_adminLineEdit->text();
}

QString Dialog::passwd()
{
    return m_passwdLineEdit->text();
}

bool Dialog::confirmPasswd(){
    return (m_passwdLineEdit->text() == m_confirmLineEdit->text());
}


