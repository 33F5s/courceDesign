#ifndef MYDIAOLOG_H
#define MYDIAOLOG_H

#include <QWidget>
#include <QDialog>
#include <QStringList>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFormLayout>
#include <QtSql/QtSql>

typedef enum {userNum=0,bookNum,borrowNum,genreNum}tableNum;

struct bookColumn{
    QString bno;
    QString bname;
    QString bauthor;
    QString bhouse;
    int bnumber;
    QString kind;
};

class myDialog : public QDialog
{
    Q_OBJECT
public:
    myDialog(tableNum table,int model,QWidget *parent=nullptr,const QStringList &list_contents=QStringList());

    void doCommand();
private:
    int model;  //模式，1更新，0插入
    QString pk;

    tableNum tableNow;

    QFormLayout *formlayout;

    QStringList list_contents;

    QString genre_kind;
    QString genre_type;
    int genre_floor=0;

    struct bookColumn book{
        .bnumber=0
    };

    void initGenre();
    void doGenreCommand();
    void initBook();
    void doBookCommand();
};

#endif // MYDIAOLOG_H