#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include <string.h>

#define ALLEGRO_COLOR_BUTTON al_map_rgb(40,140,200)
#define ALLEGRO_COLOR_DISABLED al_map_rgb(100,110,140)
#define ALLEGRO_COLOR_SELECTED al_map_rgb(0,80,255)
#define ALLEGRO_COLOR_BACKGROUND al_map_rgb(40,100,140)
#define ALLEGRO_COLOR_WHITE al_map_rgb(255,255,255)

struct botoes{
    ALLEGRO_BITMAP *imagem;
    ALLEGRO_COLOR fontc;
    ALLEGRO_FONT *fonte;
    int w,h,x,y;
    char texto[200];
    int disabled, selected;
};

struct gladiadores {
    char nome[100];
    char codigo[50];
    ALLEGRO_BITMAP *imagem;
    char folha[50];
    struct botoes *botao;
    float life;
    int lvl,STR,AGI,INT;
};

struct equipes {
    char arquivo[50];
    char nome[50];
    struct gladiadores gladiador[3];
    int selected;
};
struct equipes *equipe;
int t = 0;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_FONT *fontesm = NULL;
ALLEGRO_BITMAP *logo;
ALLEGRO_BITMAP *buffer_janela;
ALLEGRO_BITMAP *areaequipes;
ALLEGRO_BITMAP *star;
ALLEGRO_BITMAP *heart;
ALLEGRO_BITMAP *loot;
int largura_tela, altura_tela;
int largura_janela, altura_janela;
float ratio_tela, tamanho_letterbox;
float ratio_x, ratio_y;
float dpi_scale=0;
int scrollpos = 0;
int i,j,mx,my,mz;
float wlogo, hlogo;
float areaequipesw, areaequipesh;
int titlespace, itemspace, itemsize;
int borderw = 15, borderh = 10;
int scrollsize = 20;

void destroyAllegro(){
    if (buffer_janela)
        al_destroy_bitmap(buffer_janela);
    if (areaequipes)
        al_destroy_bitmap(areaequipes);
    if (star)
        al_destroy_bitmap(star);
    if (heart)
        al_destroy_bitmap(heart);
    if (logo)
        al_destroy_bitmap(logo);
    if (fila_eventos)
        al_destroy_event_queue(fila_eventos);
    if (janela)
        al_destroy_display(janela);
}

void error_msg(char *text){
	al_show_native_message_box(janela,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int initUI(){
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }

    if (!al_init_image_addon()){
        error_msg("Falha ao inicializar o addon de imagens");
        return 0;
    }

    al_init_font_addon();
    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return 0;
    }
    fonte = al_load_ttf_font("res/font/pressstart2p.ttf", 16, 0);
    fontesm = al_load_ttf_font("res/font/pressstart2p.ttf", 14, 0);
    if (!fonte || !fontesm){
        error_msg("Falha ao carregar fonte");
        return 0;
    }

    if (!al_install_keyboard()){
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }

    if (!al_init_primitives_addon()){
        error_msg("Falha ao inicializar addon de primitivas");
        return 0;
    }

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //seta tela para fullscreen
    janela = al_create_display(0, 0); //cria display. o tamanho vai ser automatico
    if(!janela) {
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_title(janela, "gladCode");
    largura_janela = al_get_display_width(janela); //captura tamanho da janela criada
    altura_janela = al_get_display_height(janela);

    largura_tela = 1360; //tamanho da tela do jogo. independe da janela criada
    altura_tela = 768;
    ratio_tela = (float)largura_tela/altura_tela; //proporcao entre largura/altura da tela do jogo
    //caso aspect ratio do jogo para o monitor seja diferente, calcula tamanho da faixa preta
    tamanho_letterbox = altura_janela - largura_janela/ratio_tela;
    altura_janela = largura_janela / ratio_tela;
    ratio_x = (float)largura_tela/largura_janela;
    ratio_y = (float)altura_tela/altura_janela;
    buffer_janela = al_create_bitmap(largura_tela, altura_tela); //buffer que seja escalado de acordo com o tamanho da janela
    if(!buffer_janela) {
        error_msg("Falha ao criar buffer da janela");
        return 0;
    }

    logo = al_load_bitmap("res/drawable/logo.png");
    if(!logo) {
        error_msg("Falha ao carregar o logo");
        return 0;
    }
    hlogo = altura_tela * 0.2;
    wlogo = hlogo * al_get_bitmap_width(logo)/al_get_bitmap_height(logo);

    titlespace = al_get_font_line_height(fonte) + 10;
    itemspace = 15;
    itemsize = 70;
    areaequipesh = altura_tela - (borderh + hlogo + itemspace + 25 + 2*itemspace + 50 + borderh);
    areaequipesw = largura_tela - 2*borderw;
    areaequipes = al_create_bitmap(areaequipesw, areaequipesh);
    if(!areaequipes) {
        error_msg("Falha ao criar area de desenho das equipes");
        return 0;
    }

    star = al_load_bitmap("res/drawable/star.png");
    if(!star) {
        error_msg("Falha ao carregar imagem star.png");
        return 0;
    }

    heart = al_load_bitmap("res/drawable/heart.png");
    if(!heart) {
        error_msg("Falha ao carregar imagem heart.png");
        return 0;
    }

    loot = al_load_bitmap("res/drawable/loot.png");
    if(!heart) {
        error_msg("Falha ao carregar imagem loot.png");
        return 0;
    }

    if (!al_install_mouse()){
        error_msg("Falha ao inicializar o mouse");
        return 0;
    }

    if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
        error_msg("Falha ao atribuir ponteiro do mouse");
        return 0;
    }

    fila_eventos = al_create_event_queue();
    if(!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    return 1;
}

struct botoes *create_button(char texto[], ALLEGRO_FONT *font, ALLEGRO_COLOR fontc, ALLEGRO_BITMAP *imagem, int x, int y, int w, int h){
    struct botoes *b = (struct botoes*)malloc(sizeof(struct botoes));
    b->w = w;
    b->h = h;
    b->x = x;
    b->y = y;
    strcpy(b->texto, texto);
    b->fonte = font;
    b->fontc = fontc;
    b->imagem = imagem;
    b->disabled = 0;
    b->selected = 0;
    return b;
}

void draw_button(struct botoes *b){
    ALLEGRO_BITMAP *bmp = al_create_bitmap(b->w, b->h);

    ALLEGRO_BITMAP *prev = al_get_target_bitmap();

    al_set_target_bitmap(bmp);

    if (b->disabled)
        al_clear_to_color(ALLEGRO_COLOR_DISABLED);
    else if (b->selected)
        al_clear_to_color(ALLEGRO_COLOR_SELECTED);
    else
        al_clear_to_color(ALLEGRO_COLOR_BUTTON);

    float w = 5;
    if (b->w - (w+5) < al_get_text_width(b->fonte, b->texto)){
        al_draw_textf(b->fonte, b->fontc, w, b->h/2 - al_get_font_line_height(b->fonte)/2, ALLEGRO_ALIGN_LEFT, "%s", b->texto);
    }
    else{
        w = b->w/2;
        al_draw_textf(b->fonte, b->fontc, w, b->h/2 - al_get_font_line_height(b->fonte)/2, ALLEGRO_ALIGN_CENTER, "%s", b->texto);
    }

    al_set_target_bitmap(prev);
    al_draw_bitmap(bmp, b->x, b->y, 0);
    al_destroy_bitmap(bmp);
}

void draw_glad_button(struct gladiadores gl){
    struct botoes *b = gl.botao;

    ALLEGRO_BITMAP *bmp = al_create_bitmap(b->w, b->h);

    ALLEGRO_BITMAP *prev = al_get_target_bitmap();

    int hpw = b->w - al_get_bitmap_width(b->imagem) - 50;
    ALLEGRO_BITMAP *hp = al_create_bitmap(hpw,5);
    int g = (gl.life-50)/50*255;
    if (gl.life > 50)
        g = 255;
    int r = (1-((gl.life-50)/50))*255;
    if (gl.life < 50)
        r = 255;
    al_set_target_bitmap(hp);
    al_clear_to_color(al_map_rgb(r,g,0));

    al_set_target_bitmap(bmp);

    if (b->disabled)
        al_clear_to_color(ALLEGRO_COLOR_DISABLED);
    else if (b->selected)
        al_clear_to_color(ALLEGRO_COLOR_SELECTED);
    else
        al_clear_to_color(ALLEGRO_COLOR_BUTTON);

    float w = b->w/2 + al_get_bitmap_width(b->imagem)/2;
    al_draw_textf(b->fonte, b->fontc, w, b->h/2 - al_get_font_line_height(b->fonte), ALLEGRO_ALIGN_CENTER, "%s", b->texto);

    al_draw_scaled_bitmap(star, 0, 0, al_get_bitmap_width(star), al_get_bitmap_height(star), al_get_bitmap_width(b->imagem), b->h/2 - 30, 30, 30, 0);
    al_draw_textf(fonte, al_map_rgb(128,128,128), al_get_bitmap_width(b->imagem) + 15 + 2, b->h/2 - al_get_font_line_height(fonte)/2 - 15 + 3, ALLEGRO_ALIGN_CENTER, "%i", gl.lvl);
    al_draw_textf(fontesm, al_map_rgb(0,0,0), al_get_bitmap_width(b->imagem) + 15, b->h/2 - al_get_font_line_height(fontesm)/2 - 15 + 2, ALLEGRO_ALIGN_CENTER, "%i", gl.lvl);

    al_draw_scaled_bitmap(hp, 0, 0, al_get_bitmap_width(hp), al_get_bitmap_height(hp), al_get_bitmap_width(b->imagem) + 25 , b->h/2 + 10, gl.life/100*hpw, al_get_bitmap_height(hp), 0);
    al_draw_scaled_bitmap(heart, 0, 0, al_get_bitmap_width(heart), al_get_bitmap_height(heart), al_get_bitmap_width(b->imagem) , b->h/2 , 30, 30, 0);

    al_draw_bitmap(b->imagem, 0, b->h/2 - al_get_bitmap_height(b->imagem)/2, 0 );

    al_set_target_bitmap(prev);
    al_draw_bitmap(bmp, b->x, b->y, 0);
    al_destroy_bitmap(bmp);
}

void draw_textbox(struct botoes *b){
    ALLEGRO_BITMAP *bmp = al_create_bitmap(b->w, b->h);

    ALLEGRO_BITMAP *prev = al_get_target_bitmap();

    al_set_target_bitmap(bmp);

    if (b->selected)
        al_clear_to_color(al_map_rgb(240,240,240));
    else
        al_clear_to_color(al_map_rgb(180,180,180));

    al_draw_textf(b->fonte, b->fontc, 10, b->h/2 - al_get_font_line_height(b->fonte)/2, ALLEGRO_ALIGN_LEFT, "%s", b->texto);

    al_set_target_bitmap(prev);
    al_draw_bitmap(bmp, b->x, b->y, 0);
    al_destroy_bitmap(bmp);
}

int isButtonClicked(struct botoes *b, int x, int y){
    if (x < b->x || x > b->x + b->w)
        return 0;
    if (y < b->y || y > b->y + b->h)
        return 0;
    if (b->disabled)
        return 0;

    return 1;
}

void captureText(char texto[], ALLEGRO_KEYBOARD_EVENT k){
    if (texto[0] >= '0' && texto[0] <= '9')
        strcpy(texto,"");
    if (k.keycode == ALLEGRO_KEY_BACKSPACE)
        texto[strlen(texto)-1] = '\0';
    else if (k.keycode == ALLEGRO_KEY_ESCAPE)
        strcpy(texto,"");
    else{
        if (k.unichar >= 32 && k.unichar <= 126){
            if (k.unichar < '0' || k.unichar > '9' || strlen(texto) > 0){
                char temp[2] = "o";
                temp[0] = k.unichar;
                strcat(texto, temp);
            }
        }
    }
}

int loadEquipe(char *name){
    char fullname[100];
    sprintf(fullname,"usr\\%s",name);
    FILE *arq = fopen(fullname,"r");
    if (!arq)
        return 1;

    if (t==0)
        equipe = (struct equipes*)malloc(sizeof(struct equipes));
    else
        equipe = (struct equipes*)realloc(equipe, sizeof(struct equipes)*(t+1));

    if (!fgets((equipe+t)->nome, 50, arq))
        return 2;

    if ((equipe+t)->nome[strlen((equipe+t)->nome)-1] == '\n')
        (equipe+t)->nome[strlen((equipe+t)->nome)-1] = '\0';

    strcpy((equipe+t)->arquivo, name);
    (equipe+t)->selected = 0;

    int j;
    for (j=0 ; j<3 ; j++){
        char linha[100];
        if (!fgets(linha, 100, arq))
            return 2;

        if (linha[strlen(linha)-1] == '\n')
            linha[strlen(linha)-1] = '\0';

        char *start, *stop;
        start = linha;
        stop = strstr(start, "  ");
        if (!stop)
            return 3;

        *stop = '\0';
        strcpy((equipe+t)->gladiador[j].nome, start);

        start = stop+2;
        stop = strstr(start, "  ");
        if (!stop)
            return 3;

        *stop = '\0';
        strcpy((equipe+t)->gladiador[j].codigo, start);

        start = stop+2;
        stop = strstr(start, "  ");
        if (stop)
            return 3;

        strcpy((equipe+t)->gladiador[j].folha, start);

        sprintf(fullname,"usr\\%s",(equipe+t)->gladiador[j].folha);
        ALLEGRO_BITMAP *folha = al_load_bitmap(fullname);
        if (!folha)
            return 4;

        (equipe+t)->gladiador[j].imagem = al_create_bitmap(64,64);
        al_set_target_bitmap((equipe+t)->gladiador[j].imagem);
        al_draw_bitmap_region(folha, 0, 650, 64, 64, 0, 0, 0);

        (equipe+t)->gladiador[j].botao = create_button((equipe+t)->gladiador[j].nome, fonte, ALLEGRO_COLOR_WHITE, (equipe+t)->gladiador[j].imagem, 0, 0, 0, 0);
        (equipe+t)->gladiador[j].life = 100;
        (equipe+t)->gladiador[j].lvl = 1;
        (equipe+t)->gladiador[j].STR = 0;
        (equipe+t)->gladiador[j].AGI = 0;
        (equipe+t)->gladiador[j].INT = 0;
    }

    fclose(arq);

    t++;
    return 0;
}

void draw_scroll(int listsize){
    ALLEGRO_BITMAP *prev = al_get_target_bitmap();

    ALLEGRO_BITMAP *sqr = al_create_bitmap(scrollsize, scrollsize);
    ALLEGRO_BITMAP *bar = al_create_bitmap(scrollsize, al_get_bitmap_height(prev));

    float barsize = (float)al_get_bitmap_height(prev)/listsize * (al_get_bitmap_height(prev) - 2*scrollsize);
    ALLEGRO_BITMAP *moving = al_create_bitmap(scrollsize, barsize);
    al_set_target_bitmap(sqr);
    al_clear_to_color(al_map_rgba(170,210,235,180));
    al_set_target_bitmap(moving);
    al_clear_to_color(al_map_rgba(50,60,80,50));
    al_set_target_bitmap(bar);
    al_clear_to_color(al_map_rgba(0,0,0,40));

    al_set_target_bitmap(prev);

    al_draw_bitmap(bar, al_get_bitmap_width(prev) - scrollsize, 0, 0);

    float bartotal = al_get_bitmap_height(prev) - 2*scrollsize - barsize;
    float barpos = (float)scrollpos/(listsize - al_get_bitmap_height(prev)) * bartotal;
    al_draw_bitmap(moving, al_get_bitmap_width(prev) - scrollsize, scrollsize + barpos, 0);
    al_draw_bitmap(sqr, al_get_bitmap_width(prev) - scrollsize, 0, 0);
    al_draw_bitmap(sqr, al_get_bitmap_width(prev) - scrollsize, al_get_bitmap_height(prev) - scrollsize, 0);

    al_destroy_bitmap(sqr);
    al_destroy_bitmap(bar);
    al_destroy_bitmap(moving);
}

int contAlive(){
    int i,j;
    int cont = 0;
    for (i=0 ; i<t ; i++){
        for (j=0 ; j<3 ; j++){
            if ( (equipe+i)->gladiador[j].life > 0 ){
                cont++;
                break;
            }
        }
    }
    return cont;
}

void killAll(){
    int i,j;
    for (i=0 ; i<t ; i++){
        for (j=0 ; j<3 ; j++)
            (equipe+i)->gladiador[j].life = 0;
    }
}

int loadUI(){

    if (rounds == 1){
        if (!initUI())
            return 0;
    }

    int sair = 0;
    int desenha = 1;

    if (estado == 0){
        char texto[50]="";

        int dpi_samples = 0, xi, yi;
        int contselected = 0;
        for (i=0 ; i<t ; i++){
            (equipe+i)->selected = 0;
            for (j=0 ; j<3 ; j++){
                (equipe+i)->gladiador[j].botao->selected = 0;
                if ((equipe+i)->gladiador[j].life <= 0){
                    (equipe+i)->gladiador[j].botao->disabled = 1;
                }
                else{
                    (equipe+i)->gladiador[j].botao->disabled = 0;
                }
            }
        }
        struct botoes *bstart = create_button("Selecione gladiadores", fonte, ALLEGRO_COLOR_WHITE, NULL, largura_tela/2 - 200, altura_tela - 50 - borderh, 400, 50);
        if (contselected == contAlive() && t > 0){
            bstart->disabled = 0;
            strcpy( bstart->texto, "Iniciar batalha");
        }
        else{
            bstart->disabled = 1;
            strcpy( bstart->texto, "Selecione gladiadores");
        }
        int scrollvisible = 0;

        while(!sair){
            while (!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);

                if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                    mx = evento.mouse.x;
                    my = evento.mouse.y;
                    mz = evento.mouse.dz;
                    if (dpi_samples < 10 && rounds == 1){ //espera alguns movimentos do mouse para calcular dpi da tela
                        if (dpi_samples == 0){
                            xi = mx; //inicialmente guarda posicao inicial do mouse
                            yi = my;
                        }
                        al_set_mouse_xy(janela, largura_janela, altura_janela); //vai para ultima posicao x y da janela
                        dpi_samples++;
                        if (dpi_samples == 10){ //ja passou 10 vezes, volta o mouse para onde tava antes desses passos
                            al_set_mouse_xy(janela, xi, yi);
                            dpi_scale = (float)largura_tela / mx; //calcula a proporcao da coordenada do mouse com o valor que temos da janela
                        }
                    }
                    mx = mx * dpi_scale; //novo x e y
                    my = (my - (tamanho_letterbox*ratio_y)/2) * dpi_scale;

                    if (mz != 0){
                        scrollpos += -40*mz;

                        if (scrollpos > t*(titlespace + itemspace + itemsize) - areaequipesh) //70 + 40 + 25
                            scrollpos = t*(titlespace + itemspace + itemsize) - areaequipesh;
                        if (scrollpos < 0)
                            scrollpos = 0;

                        desenha = 1;
                    }
                }
                else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                    contselected = 0;
                    for (i=0 ; i<t ; i++){
                        for (j=0 ; j<3 ; j++){
                            (equipe+i)->gladiador[j].botao->selected = 0;
                            if (  isButtonClicked( (equipe+i)->gladiador[j].botao, mx, my) ){
                                if (my > borderh + hlogo + 25 + 2*itemspace && my < altura_tela - (borderh + 50 + itemspace))
                                    (equipe+i)->selected = j+1;
                            }
                                //printf("%i %i %i\n ",j, (equipe+i)->gladiador[j].botao->x, (equipe+i)->gladiador[j].botao->y);
                        }
                        if ((equipe+i)->selected != 0){
                            contselected++;
                            (equipe+i)->gladiador[(equipe+i)->selected - 1].botao->selected = 1;
                        }
                    }

                    if (contselected == contAlive() && t > 1){
                        bstart->disabled = 0;
                        strcpy( bstart->texto, "Iniciar batalha");
                    }
                    else{
                        bstart->disabled = 1;
                        strcpy( bstart->texto, "Selecione gladiadores");
                    }


                    if (isButtonClicked(bstart, mx, my )){
                        nglad = contAlive();
                        sair = 1;
                        bstart->disabled = 1;
                    }

                    desenha  = 1;
                }
                else if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
                    switch(evento.keyboard.keycode){
                        case ALLEGRO_KEY_UP:
                            scrollpos -= 40;
                            break;
                        case ALLEGRO_KEY_DOWN:
                            scrollpos += 40;
                            break;

                    }
                    if (scrollpos > t*(titlespace + itemspace + itemsize) - areaequipesh) //70 + 40 + 25
                        scrollpos = t*(titlespace + itemspace + itemsize) - areaequipesh;
                    if (scrollpos < 0)
                        scrollpos = 0;

                    if (rounds == 1){
                        captureText(texto, evento.keyboard);

                        if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER){
                            int r = loadEquipe(texto);
                            if (r == 0)
                                strcpy(texto,"");
                            else if (r == 1)
                                strcpy(texto,"1Arquivo nao encontrado");
                            else if (r == 2)
                                strcpy(texto,"2Erro na leitura do arquivo");
                            else if (r == 3)
                                strcpy(texto,"3Erro no formato do arquivo");
                            else if (r == 4)
                                strcpy(texto,"4Erro ao carregar imagem");

                            if (contselected == contAlive() && t > 1){
                                bstart->disabled = 0;
                                strcpy( bstart->texto, "Iniciar batalha");
                            }
                            else{
                                bstart->disabled = 1;
                                strcpy( bstart->texto, "Selecione gladiadores");
                            }
                        }
                    }

                    if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                        killAll();
                        sair = 1;
                        return 0;
                    }
                    desenha = 1;
                }
                else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    sair = 1;
                }
            }

            if (desenha){
                int y = borderh;
                int x = borderw;
                int h = itemsize;

                al_set_target_bitmap(buffer_janela);
                al_clear_to_color(ALLEGRO_COLOR_BACKGROUND);

                al_draw_scaled_bitmap(logo, 0, 0,
                                      al_get_bitmap_width(logo), al_get_bitmap_height(logo),
                                      largura_tela/2 - wlogo/2, y,
                                      wlogo, hlogo, 0);
                y += hlogo + itemspace;

                if (rounds == 1){
                    struct botoes *textbox;
                    if (strcmp(texto,"")==0){
                        textbox = create_button("Arquivo de configuracao da equipe...", fonte, al_map_rgb(70,70,70), NULL, largura_tela/2 - largura_tela/3, y, largura_tela/3*2, 25);
                    }
                    else if (texto[0] >= '0' && texto[0] <= '9'){
                        textbox = create_button(texto+1, fonte, al_map_rgb(180,20,0), NULL, largura_tela/2 - largura_tela/3, y, largura_tela/3*2, 25 );
                        textbox->selected = 1;
                    }
                    else{
                        textbox = create_button(texto, fonte, al_map_rgb(0,0,0), NULL, largura_tela/2 - largura_tela/3, y, largura_tela/3*2, 25 );
                        textbox->selected = 1;
                    }
                    draw_textbox(textbox);
                }

                y += 25 + itemspace;

                al_set_target_bitmap(areaequipes);
                al_clear_to_color(ALLEGRO_COLOR_BACKGROUND);
                //al_clear_to_color(al_map_rgb(40,40,40));
                int y2 = 0, x2 = 0;
                if (t*(titlespace + itemspace + itemsize) > areaequipesh){
                    draw_scroll(t*(titlespace + itemspace + itemsize));
                    scrollvisible = 1;
                }
                for (i=0 ; i<t ; i++){
                    al_draw_textf(fonte, ALLEGRO_COLOR_WHITE, x2, y2 - scrollpos, ALLEGRO_ALIGN_LEFT, "%s", (equipe+i)->nome);
                    y2 += titlespace;
                    for (j=0 ; j<3 ; j++){
                        (equipe+i)->gladiador[j].botao->x = x2;
                        (equipe+i)->gladiador[j].botao->y = y2 - scrollpos;
                        if (scrollvisible)
                            (equipe+i)->gladiador[j].botao->w = (areaequipesw-10-scrollsize-5)/3;
                        else
                            (equipe+i)->gladiador[j].botao->w = (areaequipesw-10)/3;
                        (equipe+i)->gladiador[j].botao->h = h;
                        draw_glad_button((equipe+i)->gladiador[j]);
                        (equipe+i)->gladiador[j].botao->x = x + x2;
                        (equipe+i)->gladiador[j].botao->y = y + y2 - scrollpos;
                        x2 += (equipe+i)->gladiador[j].botao->w + 5;
                    }
                    y2 += h + itemspace;
                    x2 = 0;
                }

                al_set_target_bitmap(buffer_janela);

                al_draw_bitmap_region(areaequipes, 0, 0,
                                      areaequipesw, areaequipesh,
                                       x, y, 0);

                //al_draw_textf(fonte, ALLEGRO_COLOR_WHITE, largura_tela - 570, bstart->y - 50, ALLEGRO_ALIGN_RIGHT, "%i %i %.3f %i", mx,my,dpi_scale,mz);
                //al_draw_textf(fonte, ALLEGRO_COLOR_WHITE, largura_tela - 570, bstart->y - 50, ALLEGRO_ALIGN_RIGHT, "%i %i %i", winners[0], winners[1], winners[2]);

                if (t > 1)
                    draw_button(bstart);

                al_set_target_backbuffer(janela);
                al_draw_scaled_bitmap(buffer_janela, 0, 0, largura_tela, altura_tela, 0, tamanho_letterbox/2, largura_janela-15, altura_janela-20, 0 );

                al_flip_display();
                desenha = 0;
            }
        }
    }
    else if (estado == 1){
        sair = 0;
        desenha = 1;
        while (!sair){
            while (!al_is_event_queue_empty(fila_eventos)){
                ALLEGRO_EVENT evento;
                al_wait_for_event(fila_eventos, &evento);

                if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
                    desenha = 1;
                }
                else if(evento.type == ALLEGRO_EVENT_KEY_CHAR) {
                    if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                        sair = 1;
                        estado++;
                    }
                }
                else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    sair = 1;
                }

            }

            if (desenha && !sair){
                int y = borderh;
                int x;
                itemspace = 30;

                al_set_target_bitmap(buffer_janela);
                al_clear_to_color(ALLEGRO_COLOR_BACKGROUND);

                al_draw_scaled_bitmap(logo, 0, 0,
                                      al_get_bitmap_width(logo), al_get_bitmap_height(logo),
                                      largura_tela/2 - wlogo/2, y,
                                      wlogo, hlogo, 0);
                y += itemspace;

                ALLEGRO_BITMAP *square = al_create_bitmap(largura_tela/3, altura_tela/2);
                al_set_target_bitmap(square);
                al_clear_to_color(al_map_rgb(90,90,130));


                char str[50] = "EQUIPES VENCEDORAS";
                x = al_get_bitmap_width(square)/2 - al_get_text_width(fonte, str)/2;
                al_draw_textf(fonte, al_map_rgb(0,0,0), x, y, ALLEGRO_ALIGN_LEFT, str);
                y += itemspace + al_get_font_line_height(fonte) + 50;

                ALLEGRO_COLOR colors[3] = {al_map_rgb(219,213,98),al_map_rgb(203,203,203),al_map_rgb(182,140,75)};

                for (i=0 ; i<3 ; i++){
                    if (winners[i] != -1){
                        x = al_get_bitmap_width(square)/2 - (al_get_text_width(fonte, (equipe+winners[i])->nome) + 120)/2;
                        al_draw_scaled_bitmap(loot, i*32, 0, 32, 32, x, y - 25, 50, 50, 0);
                        x += 60 ;
                        al_draw_textf(fonte, colors[i], x, y - al_get_font_line_height(fonte)/2, ALLEGRO_ALIGN_LEFT, "%s", (equipe+winners[i])->nome );
                        x += al_get_text_width(fonte, (equipe+winners[i])->nome) + 10;
                        al_draw_scaled_bitmap(loot, i*32, 0, 32, 32, x, y - 25, 50, 50, 0);
                        y += itemspace + 50;
                    }
                }

                al_set_target_bitmap(buffer_janela);
                //al_draw_textf(fonte, ALLEGRO_COLOR_WHITE, largura_tela/2, altura_tela-100, ALLEGRO_ALIGN_CENTER, "%i %i %i", winners[0], winners[1], winners[2]);
                al_draw_bitmap(square, largura_tela/2 - al_get_bitmap_width(square)/2, altura_tela/2 - al_get_bitmap_height(square)/2, 0);

                al_set_target_backbuffer(janela);
                al_draw_scaled_bitmap(buffer_janela, 0, 0, largura_tela, altura_tela, 0, tamanho_letterbox/2, largura_janela-15, altura_janela-20, 0 );
                al_flip_display();
                desenha = 0;
            }
        }
    }

    return 1;
}

