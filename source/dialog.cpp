#include "../include/dialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QtSql>

Dialog::Dialog(userAction ua, QWidget *parent,int permi)
    : QDialog(parent)
{
    this->permission=permi;
    this->ua=ua;
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttonBox->button(QDialogButtonBox::Ok)->setFocus();

    switch(ua){
        case Login:setWindowTitle("登录");break;
        case Register:setWindowTitle("注册");break;
        case ChangePassword:setWindowTitle("修改密码");break;
        case DeleteUser:setWindowTitle("注销账号");break;
        default:break;
    }

    if(ua==Login || ua==DeleteUser){
        setFixedSize(280, 180);

        m_buttonBox->setFixedSize(180, 40);
        m_buttonBox->move(30, 120);
    }
    else {
        setFixedSize(280, 220);

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

    connect(m_buttonBox, &QDialogButtonBox::accepted, [&]{
        switch(this->ua){
            case Login:LoginSlot();break;
            case Register:RegisterSlot();break;
            case ChangePassword:ChangePasswordSlot();break;
            case DeleteUser:DeleteUserSlot();break;
            default:break;
        }
    });
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

int Dialog::getPermission(){
    return this->permission;
}

void Dialog::setAdmin(const QString &admin){
    m_adminLineEdit->setText(admin);
}

void Dialog::setNoEditedAdmin(){
    m_adminLineEdit->setReadOnly(true);
}

void Dialog::LoginSlot(){
    QSqlQuery qry;
    qry.prepare("select permission from user where name=:name and passwd=:passwd");
    qry.bindValue(":name",m_adminLineEdit->text());
    qry.bindValue(":passwd",m_passwdLineEdit->text());
    if(!qry.exec()){
        qDebug()<<qry.lastError().text();
        QMessageBox::warning(this,"警告","查询出错");
        return;
    }
    if(!qry.next()){
        QMessageBox::warning(this,"警告","请检查账号密码");
        return;
    }
    this->permission=qry.value(0).toInt();
    QDialog::accept();
}

void Dialog::RegisterSlot(){
    QSqlQuery qry;
    if(m_passwdLineEdit->text().isEmpty() || m_adminLineEdit->text().isEmpty() || m_confirmLineEdit->text().isEmpty()){
        QMessageBox::warning(this,"警告","密码或用户名为空");
        return;
    }
    if(m_passwdLineEdit->text() != m_confirmLineEdit->text()){
        QMessageBox::warning(this,"警告","两次密码不一致");
        return;
    }
    qry.prepare("insert into user(name,passwd,permission)values(:name,:passwd,:permission)");
    qry.bindValue(":name",m_adminLineEdit->text());
    qry.bindValue(":passwd",m_passwdLineEdit->text());
    qry.bindValue(":permission",this->permission);
    if(qry.exec()){
        QMessageBox::information(this,"提示","注册成功");
        QDialog::accept();
    }
    else{
        qDebug()<<qry.lastError().text();
        QMessageBox::warning(this,"警告","注册失败（账号名重复）");
    }
}

void Dialog::ChangePasswordSlot(){
    QSqlQuery qry;
    if(m_passwdLineEdit->text().isEmpty() || m_adminLineEdit->text().isEmpty() || m_confirmLineEdit->text().isEmpty()){
        QMessageBox::warning(this,"警告","密码或用户名为空");
        return;
    }
    if(m_passwdLineEdit->text() != m_confirmLineEdit->text()){
        QMessageBox::warning(this,"警告","两次密码不一致");
        return;
    }
    qry.prepare("select permission from user where name=:name");
    qry.bindValue(":name",m_adminLineEdit->text());
    if(!qry.exec() || !qry.next()){
        qDebug()<<qry.lastError().text();
        QMessageBox::warning(this,"警告","查询失败");
        return;
    }
    if(permission<=qry.value(0).toInt()){
        QMessageBox::warning(this,"警告","权限不足");
        return;
    }
    qry.prepare("update user set passwd=:passwd where name=:name");
    qry.bindValue(":name",m_adminLineEdit->text());
    qry.bindValue(":passwd",m_passwdLineEdit->text());
    if(qry.exec() && qry.numRowsAffected()>0){
        QMessageBox::information(this,"提示","修改成功");
        QDialog::accept();
    }
    else{
        qDebug()<<qry.lastError().text();
        QMessageBox::warning(this,"警告","修改失败");
    }
}

void Dialog::DeleteUserSlot(){
    QSqlQuery qry;
    if(m_passwdLineEdit->text().isEmpty() || m_adminLineEdit->text().isEmpty()){
        QMessageBox::warning(this,"警告","密码或用户名为空");
        return;
    }
    qry.prepare("select permission from user where name=:name");
    qry.bindValue(":name",m_adminLineEdit->text());
    if(!qry.exec() || !qry.next()){
        qDebug()<<qry.lastError().text();
        QMessageBox::warning(this,"警告","查询失败");
        return;
    }
    if(permission<=qry.value(0).toInt()){
        QMessageBox::warning(this,"警告","权限不足");
        return;
    }
    qry.prepare("delete from user where name=:name and passwd=:passwd");
    qry.bindValue(":name",m_adminLineEdit->text());
    qry.bindValue(":passwd",m_passwdLineEdit->text());
    if(qry.exec() && qry.numRowsAffected()>0){
        QMessageBox::information(this,"提示","注销成功");
        QDialog::accept();
    }
    else{
        qDebug()<<qry.lastError().text();
        QMessageBox::warning(this,"警告","注销失败(检查账号密码以及是否未还书)");
    }
}
