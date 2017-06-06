#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("B.Rog");
    setSTR(3);
    setAGI(9);
    setINT(3);
}

int hpant = 1000, startcombo = 1;
loop(){
    if (howManyEnemies() == 0){
        if (getMyHp() < hpant){
            hpant = getMyHp();
            turnToHeading(whereThatCameFrom());
        }
        else{
            moveTo(screenW/2, screenH/2);
        }
    }
    else{
        float x,y;
        getLowHp(&x,&y);
        lockOnTarget(x, y);
        if (startcombo && getMyAp() >= 30){
            ambush();
            startcombo = 0;
        }
        else if (getAmbushTimeLeft() > 0.3){
            float rh = getLockedTargetRelativeHeading();
            if (rh > 30 && rh < 330){
                if (rh <= 180){
                    moveLeft();
                    moveTo(x,y);
                }
                else{
                    moveRight();
                    moveTo(x,y);
                }
            }
            else {
                moveTo(x,y);
            }
        }
        else{
            float rh = getLockedTargetRelativeHeading();
            if (isLockedTargetVisible() && (rh < 150 || rh > 210)){
                attackRanged(x,y);
                if (getMyAp() >= 20 && isLockedTargetVisible()){
                    assassinate();
                    startcombo = 1;
                }
            }
            else{
                ambush();
            }
        }
    }
}
