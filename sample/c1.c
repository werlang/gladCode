#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("Tutorius");
    setSTR(5);
    setAGI(7);
    setINT(3);
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
        if (getDist(x,y) <= 10){
            attackRanged(x,y);
        }
        else
            moveTo(x,y);
    }
}
