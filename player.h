#ifndef PLAYER_H
#define PLAYER_H
/*sluzi za cuvanje trenutnog i ciljnog stanja*/
typedef struct{
    float curr;
    float goal;
}Val2f;
/*struktura za sve kockaste objekte
* posx,y,z predstavljaju centar tela*/
typedef struct{
    float posx;
    float posy;
    float posz;
    Val2f vx;
    Val2f vy;
    Val2f vz;
    float length;
    float height;
    float width;
    float color[3];
}Object;

typedef struct{
    int jumping;

}State;
extern State state;
/*relativna pozicija visine glave u odnosu na centar tela
* sluzi za pozicioniranje kamere*/
extern float playerHeadHeight;
extern const Object playerInit;
extern void movePlayer();

extern Object player;

extern float rotWorld;
extern Val2f viewAzimuth;
extern Val2f viewElevation;
extern float JUMP_V;
extern Object player;
extern int animationOngoing;

extern float rotWorld;
extern Val2f viewAzimuth;
extern Val2f viewElevation;
extern float eyex, eyey, eyez;
extern float lookAtx, lookAty, lookAtz;
extern float upx, upy, upz;


#endif
