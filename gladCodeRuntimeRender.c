#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

float screenRate = 25;
float screenW = 25;
float screenH = 25;
float timeInterval = 0.01;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;

void error_msg(char *text){
	al_show_native_message_box(NULL,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int renderInit(){
    g = (struct gladiador*)malloc(sizeof(struct gladiador)*(nglad));

    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    if (!al_init_primitives_addon()){
        error_msg("Falha ao inicializar addon de primitivas");
        return 0;
    }

    //cria o timer com o intervalo de tempo que ele ira disparar
    timer = al_create_timer(timeInterval);
    if(!timer) {
        error_msg("Falha ao criar temporizador");
        return 0;
    }

    janela = al_create_display(screenRate * screenW, screenRate * screenH);
    if(!janela) {
        error_msg("Falha ao criar janela");
        al_destroy_timer(timer);
        return 0;
    }

    al_set_window_title(janela, "gladCode runtime C Render");

    fila_eventos = al_create_event_queue();
    if(!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    return 1;
}

int desenha = 1;
int sair = 0;

int renderLoop(){
    while(!sair){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if(evento.type == ALLEGRO_EVENT_TIMER){
            loadStructFromMemory();
            if (kbhit())
                sair=1;
            desenha = 1;
        }
        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
        }

        if(desenha && al_is_event_queue_empty(fila_eventos)) {
            int i;
            al_clear_to_color(al_map_rgb(0,0,0));
            for (i=0 ; i<nglad ; i++){
                //if ((g+i)->hp > 0){
                    //printf("%s %.1f %.1f\n",(g+i)->name, (g+i)->x, (g+i)->y);
                    float r = 10;
                    al_draw_circle((int)((g+i)->x * screenRate), (int)((g+i)->y * screenRate), r, al_map_rgb(255,255,255), 1);
                    float x = sin( (g+i)->head * M_PI/180 ) * r;
                    float y = -cos( (g+i)->head * M_PI/180 ) * r;
                    al_draw_circle((int)((g+i)->x * screenRate) + x, (int)((g+i)->y * screenRate) + y, r/3, al_map_rgb(0,255,0), 1);
                //}
            }

            al_flip_display();
            desenha = 0;
        }
    }

    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    return 0;
}
