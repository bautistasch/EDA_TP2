//
//  main.c
//  Test
//
//  Created by marcela tinayre on 3/16/21.
//

#define OFFSET 100

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>


int main(int argc, char * argv[]){
    al_init();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    
    
    srand((unsigned int)time(NULL));

    
    unsigned int width, height;
    unsigned int nRobots;
    
    width = (rand()%800)+800;
    height = (rand()%600)+800;
    
    nRobots = (rand()%5)+5
    ;
    printf("width: %d, height: %d, nRobots: %d\n", width, height, nRobots);
    
    unsigned int scale = 1;
    if (nRobots < 15) {
        scale = 2;
    }
    
    ALLEGRO_DISPLAY * display;
    ALLEGRO_EVENT_QUEUE * queue;
    ALLEGRO_FONT * font;
    al_install_keyboard();
    
    queue = al_create_event_queue();
    display = al_create_display(width, height);
    font = al_load_font("arial.ttf", 600/(nRobots * scale), 0);
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    ALLEGRO_EVENT event;
    
    //Grafico
    //ejes
    al_draw_line(0.0 + OFFSET, 0.0, 0.0 + OFFSET, height - OFFSET, al_map_rgb(0, 255, 0), 2.0);
    al_draw_line(0.0 + OFFSET, height - OFFSET, width, height - OFFSET, al_map_rgb(0, 255, 0), 2.0);
    al_draw_text(font, al_map_rgb(0, 255, 0), 0.66 * OFFSET, height - OFFSET , ALLEGRO_ALIGN_CENTER, "0");
    
    // puntos del grafico y marcas en el eje x
    unsigned int i = 1;
    unsigned int x1 = OFFSET + (((width -OFFSET )/nRobots) * i);
    unsigned int y1 = (rand()%(height-OFFSET));
    al_draw_line(x1, height - OFFSET, x1, height - OFFSET + 10, al_map_rgb(0, 255, 0), 10);
    al_draw_textf(font, al_map_rgb(0, 255, 0), x1  , height - OFFSET + 30, ALLEGRO_ALIGN_CENTER, "%d", i);
    
    unsigned int masArriba = y1, masAbajo = y1;
    unsigned int medio;
    
    
    i++;
    for (  ; i <= nRobots; i++) {
        unsigned int x2 = OFFSET + (((width -OFFSET )/nRobots) * i);
        unsigned int y2 = (rand()%(height-OFFSET));
        al_draw_line(x2, height - OFFSET, x2, height - OFFSET + 10, al_map_rgb(0, 255, 0), 10);
        al_draw_textf(font, al_map_rgb(0, 255, 0), x2, height - OFFSET + 30, ALLEGRO_ALIGN_CENTER, "%d", i);
        
        
        if (masArriba > y2 ) {
            masArriba = y2;
        }
        if(masAbajo < y2){
            masAbajo = y2;
        }
        al_draw_filled_circle((float) x1, (float) y1, 5.0, al_map_rgb(255, 0, 0));

        al_draw_line(x1, y1, x2, y2, al_map_rgb(255, 255, 255), 3);
        x1 = x2;
        y1 = y2;
    }
    al_draw_filled_circle((float) x1, (float) y1, 5.0, al_map_rgb(255, 0, 0));
 
    
    
    //Marcas en los ejes
    al_draw_textf(font, al_map_rgb(0, 255, 0), 0.66 * OFFSET - 10, masAbajo - scale*30/nRobots, ALLEGRO_ALIGN_CENTER, "%d", height-(OFFSET + masAbajo));
    al_draw_line(OFFSET - 10, masArriba, OFFSET, masArriba, al_map_rgb(0, 255, 0), 10);
    
    al_draw_textf(font, al_map_rgb(0, 255, 0), 0.66 * OFFSET - 10, masArriba - scale*30/nRobots, ALLEGRO_ALIGN_CENTER, "%d", height-(OFFSET+masArriba));
    al_draw_line(OFFSET - 10, masAbajo, OFFSET, masAbajo, al_map_rgb(0, 255, 0), 10);
    
    medio = (masArriba+masAbajo)/2;
    al_draw_textf(font, al_map_rgb(0, 255, 0), 0.66 * OFFSET - 10, medio - scale*30/nRobots, ALLEGRO_ALIGN_CENTER, "%d", height-(OFFSET+medio));
    al_draw_line(OFFSET - 10, medio, OFFSET, medio, al_map_rgb(0, 255, 0), 10);
    
    
    al_flip_display();
    
    
    bool running = true;
    while (running) {
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_UP || event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
    }
    al_uninstall_keyboard();
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    al_destroy_font(font);
    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    
    return 0;
}
    

