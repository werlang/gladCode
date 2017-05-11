#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("PunyAss");
    setSTR(3);
    setAGI(8);
    setINT(4);
    upgradeAGI();
}

loop(){
    if (howManyEnemies() == 0){
        moveForward();
        if (getMyX() > 20 || getMyX() < 5 || getMyY() > 20 || getMyY() < 5){
            turn(2.0);
        }
    }
    else{
        if (getCamouflageTimeLeft() <= 0 && getMyAp() >= 25)
            camouflage();
        else{
            float x,y;
            getLowHp(&x,&y);
            lockOnTarget(x, y);
            if (getDist(x,y) > 2)
                moveTo(x,y);
            else if (getMyAp() >= 10)
                assassinate(x,y);
            else
                attackRanged(x,y);
        }
    }
}
