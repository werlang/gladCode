#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("Wanderer");
    setSTR(5);
    setAGI(10);
    setINT(0);
    upgradeSTR();
}

int point = 0;

loop(){
    if (point%4 == 0){
        if ((int)getMyX() != 3 || (int)getMyY() != 3)
            moveTo(3,3);
        else
            point = (point+1)%4;
    }
    else if (point%4 == 1){
        if ((int)getMyX() != 22 || (int)getMyY() != 3)
            moveTo(22,3);
        else
            point = (point+1)%4;
    }
    if (point%4 == 2){
        if ((int)getMyX() != 22 || (int)getMyY() != 22)
            moveTo(22,22);
        else
            point = (point+1)%4;
    }
    if (point%4 == 3){
        if ((int)getMyX() != 3 || (int)getMyY() != 22)
            moveTo(3,22);
        else
            point = (point+1)%4;
    }
}
