#ifndef OPTION_H
#define OPTION_H
#include <QDate>
namespace option{
    typedef enum{bothUser=0,normalUser,rootUser}userType;
    typedef enum{allState=0,borrowed,returned,NotReturned}borrowState;
    typedef enum{ASC=0,DESC}orderBy;
    typedef enum{C_RETURN_TIME,C_BORROW_TIME,C_MONEY,C_TIME_CNT}orderColumn;

    struct userOption{
        userType type;
        borrowState state;
    };

    struct bookOption{
        int minCnt;
        int maxCnt;
        orderBy order;
    };

    struct grenreOption{
        int minFloor;
        int maxFloor;
    };

    struct borrowOption{
        borrowState state;
        orderColumn o_column;
        orderBy order;
        QDate borrowStartDate;
        QDate borrowEndDate;
        QDate returnStartDate;
        QDate returnEndDate;
        int timeCntStart;
        int timeCntEnd;
        int moneyStart;
        int moneyEnd;
    };
}
#endif // OPTION_H