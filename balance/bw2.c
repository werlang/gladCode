#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("B.War");
    setSTR(9);
    setAGI(4);
    setINT(2);

}

int hpant = 1000;
loop(){
    float x, y;
    if ( !isLockedTargetVisible() ){
        if ( howManyEnemies() > 0 ){
            getLowHp(&x, &y);
            lockOnTarget(x,y);
            if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0 && getMyAp() >= 25){
                block();
            }
            if (getMyAp() >= 15 && getDist(x,y) > 2){
                charge();
            }
            else
                moveTo(x,y);
        }
        else{
            if (getBlockTimeLeft() <= 0 && getMyAp() >= 25){
                block();
            }
            else if (getMyHp() < hpant){
                hpant = getMyHp();
                turnToHeading(whereThatCameFrom());
            }
            else{
                moveTo(screenW/2, screenH/2);
            }
        }
    }
    else{
        getLockedTargetCoords(&x,&y);
        if (getDist(x,y) <= 1)
            attackMelee(x,y);
        else if (getMyAp() >= 15 && getDist(x,y) > 2){
            charge();
        }
        else if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0 && getMyAp() >= 25){
            block();
        }
        else
            moveTo(x,y);
    }
}
