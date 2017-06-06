#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("B.Mag");
    setSTR(5);
    setAGI(0);
    setINT(10);
    upgradeINT();

}

int hpant = 1000;

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
        if (getCloseEnemy(&x,&y)){
            if (getDist(x,y) < 2.5){
                if (getDist(screenW/2, 0) > 5 && getMyAp() >= 30)
                    teleport(screenW/2, 0);
                else if  (getDist(screenW/2, screenH) > 5 && getMyAp() >= 30)
                    teleport(screenW/2, screenH);
                else{
                    moveBackwards();
                }
            }
            else{
                if (getMyAp() >= 20){
                    fireball(x,y);
                }
                else{
                    attackRanged(x,y);
                }
            }
        }
        else{
            moveTo(screenW/2, screenH/2);
        }
    }
}
