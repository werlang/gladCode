#include "gladCodeIO.c"

setup(){
    registerGlad();
    setName("Testee");
    setSTR(4);
    setAGI(5);
    setINT(6);
}

loop(){
    float x, y;
    if ( !isLockedTargetVisible() ){
        if ( howManyEnemies() > 0 ){
            getLowHp(&x, &y);
            lockOnTarget(x, y);
        }
        else{
            if (getMyX() < 5)
                turnTo(25, getMyY());
            if (getMyX() > 20)
                turnTo(0, getMyY());
            if (getMyY() < 5)
                turnTo(getMyX(), 25);
            if (getMyY() > 20)
                turnTo(getMyX(), 0);
            moveForward();
        }
    }
    else{
        getLockedTargetCoords(&x,&y);
        moveTo(x,y);
    }
}
