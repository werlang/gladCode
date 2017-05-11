#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("Brutus");
    setSTR(7);
    setAGI(5);
    setINT(3);
}

loop(){
    float x, y;
    if ( !isLockedTargetVisible() ){
        if ( howManyEnemies() > 0 ){
            getLowHp(&x, &y);
            lockOnTarget(x,y);
            if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0)
                block();
            charge();
        }
        else{
            turn(60);
        }
    }
    else{
        getLockedTargetCoords(&x,&y);
        if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0)
            block();
        charge();
    }
}
