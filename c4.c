#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("Magus");
    setSTR(2);
    setAGI(4);
    setINT(9);
    upgradeINT();
}

loop(){
    if (getMyX() != 12 || getMyY() != 12){
        if (getMyAp() >= 30){
            teleport(12,12);
        }
        else{
            moveTo(12, 12);
        }
    }
    else{
        float x,y;
        if (getLowHp(&x,&y)){
            if (getMyAp() >= 20){
                fireball(x,y);
            }
            else{
                attackRanged(x,y);
            }
        }
        else{
            turn(60);
        }
    }
}
