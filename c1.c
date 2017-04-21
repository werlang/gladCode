#include "gladCodeIO.c"

setup(){
    registerGlad();
    setName("Tutorius");
    setSTR(7);
    setAGI(5);
    setINT(3);
    upgradeSTR();
}

loop(){
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
        //printf("%.2f ",getLockedTargetSpeed());
        //printf("%i ",doYouSeeMe(x,y));
        if (getDist(x,y) <= 10)
            attackRanged(x,y);
        else
            moveTo(x,y);
    }
}
