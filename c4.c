#include "gladCodeIO.c"

setup(){
    registerGlad();
    setName("Dummy");
    setSTR(5);
    setAGI(5);
    setINT(5);
    upgradeSTR();
}

loop(){
    moveForward();
    turn(1.0);
}
