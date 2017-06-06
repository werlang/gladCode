#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("PunyAss");
    setSTR(3);
    setAGI(8);
    setINT(4);
    upgradeAGI();
}

int hpant = 1000;

loop(){
    if (getMyHp() < hpant){
        hpant = getMyHp();
        turnToHeading(whereThatCameFrom());
    }
    if (howManyEnemies() == 0){
        moveForward();
        if (getMyX() > 20 || getMyX() < 5 || getMyY() > 20 || getMyY() < 5){
            turn(2.0);
        }
    }
    else{
        float x,y;
        getLowHp(&x,&y);
        lockOnTarget(x, y);
        if (getAmbushTimeLeft() <= 0 && getMyAp() >= 35)
            ambush();
        else{
            if (getDist(x,y) > 2 && getMyAp() >= 15)
                moveTo(x,y);
            else if (getMyAp() >= 15)
                assassinate();
            else
                attackRanged(x,y);
        }
    }
}
