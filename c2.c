#include "gladCodeIO.c"

setup(){
    registerGlad();
    setName("Novatus");
    setSTR(3);
    setAGI(9);
    setINT(3);
}

int point = 0;

loop(){
    if (point%2){
        if (getMyX() != 10 || getMyY() != 3)
            moveTo(10,3);
        else
            point++;
    }
    else{
        if (getMyX() != 12 || getMyY() != 21)
            moveTo(12,21);
        else
            point++;
    }
}
