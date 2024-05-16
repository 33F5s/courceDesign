#ifndef OPTION_H
#define OPTION_H
namespace option{
    typedef enum{bothUser=0,normalUser,rootUser}userType;
    typedef enum{bothState=0,borrowed,returned}borrowState;

    struct userOption{
        userType type;
        borrowState state;
    };

    struct bookOption{
        int minCnt;
        int maxCnt;
    };

    struct grenreOption{
        int minFloor;
        int maxFloor;
    };
}
#endif // OPTION_H