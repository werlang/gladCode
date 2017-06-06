#include "gladCodeAPI.c"

setup(){
    registerGlad();
    setName("Magus");
    setSTR(2);
    setAGI(4);
    setINT(9);
    upgradeINT();
}

int hpant = 1000;
int flip=0;

loop(){
    if (howManyEnemies() == 0){
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
    else{
        float x,y;
        if (getCloseEnemy(&x,&y)){
            if (getDist(x,y) < 2.5)
                if (getDist(0,0) > 10 && getMyAp() >= 30)
                    teleport(0,0);
                else if  (getDist(25,25) > 10 && getMyAp() >= 30)
                    teleport(25,25);
                else{
                    moveBackwards();
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
            turn(60);
        }
    }
}
