#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("Brutus");
    setSTR(7);
    setAGI(5);
    setINT(3);
}

int hpant = 1000;
int flip=0;

loop(){
    float x, y;
    if ( !isLockedTargetVisible() ){
        if ( howManyEnemies() > 0 ){
            getLowHp(&x, &y);
            lockOnTarget(x,y);
            if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0 && getMyAp() >= 25)
                block();
            if (getMyAp() >= 10)
                charge();
        }
        else{
            if (getMyHp() < hpant){
                hpant = getMyHp();
                turnToHeading(whereThatCameFrom());
            }
            else{
                if (flip)
                    moveTo(25,25);
                else
                    moveTo(0,0);
                if ((getMyX() == 0 && getMyY() == 0) || (getMyX() == 25 && getMyY() == 25) )
                    flip = (flip+1)%2;
            }
        }
    }
    else{
        getLockedTargetCoords(&x,&y);
        if (doYouSeeMe(x,y) && getBlockTimeLeft() <= 0 && getMyAp() >= 25)
            block();
        else if (getDist(x,y) <= 1)
            attackMelee(x,y);
        else if (getMyAp() >= 10)
            charge();
        else
            moveTo(x,y);
    }
}
