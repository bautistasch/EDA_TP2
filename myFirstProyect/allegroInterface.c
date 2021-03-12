#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "parseArg.h"


#include <allegro5/allegro.h>      
#include <allegro5/allegro_color.h> 
#include <allegro5/allegro_primitives.h> 

#define FPS 1.00 

#define TILE_SIZE 50          // ESTO EL USUARIO LO INGRESA A MANO

#define D_WIDTH 500
#define D_HEIGHT 500

#define MAXSIM 1000
#define PI 3.14159265
enum { CLEAN, DIRTY };

typedef struct
{
	double x;
	double y;
	double angle;
}Robot;

typedef struct
{
	int width;            // yo le paso el width y height normalizado
	int height;
	bool* tiles;
}Floor;

typedef struct
{
	Robot* robs;
	Floor* f;
	int robotCount;
	long tickCount;
}Sim;

typedef struct
{
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* event_queue;
	ALLEGRO_TIMER* timer;
}AllegroTypes;

typedef struct
{
	int width;
	int height;
	int robots;
}dataInput;


// ######################################
// #### PROTOTIPOS ######################
// ######################################

Floor* createFloor(int width, int height);
Robot* createRobots(int width, int height, int n);
void deleteRobots(Robot* robots);
Sim* createSim(int width, int height, int robotCount);
void deleteSim(Sim* s);
// para la simulacion de 1000, no lo use todavia
int simulate(Sim* s);
void onTick(Robot* r, Floor* f);
bool inMatrix(Floor* f, double x, double y);
bool chkWin(Floor* f);
AllegroTypes* initAllegro(void);
void destroyAllegro(AllegroTypes* myPtrs);
// cambiar nombre de play, lo que hace es jugar un tick solo
void play(Sim* s);
void drawSim(Sim* s);
// tranforma coordenadas de back a front
void back2Front(Sim* s, double* x, double* y);

int parseCallback(char* key, char* value, void* userData);

int main(int argc, char* argv [])
{
	dataInput myDataInput;                                          // VER COMO INGRESAR COMANDO POR VS
	parseCmdLine(argc, argc, parseCallback, &myDataInput );        // No lo puedo probar

	AllegroTypes* allPtrs = initAllegro();

	Sim* mySim = createSim(10, 10, 3);  // piso de 10 x 10, 3 aspiradoras

	bool redraw = false;
	bool display_close = false;

	while (!display_close) {
		ALLEGRO_EVENT ev;
		if (al_get_next_event(allPtrs->event_queue, &ev)) //Toma un evento de la cola, VER RETURN EN DOCUMENT.
		{
			if (ev.type == ALLEGRO_EVENT_TIMER)
				redraw = true;

			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				display_close = true;
		}

		if (redraw && al_is_event_queue_empty(allPtrs->event_queue)) {
			redraw = false;
			play(mySim);
			drawSim(mySim);
		}
	}
	deleteSim(mySim);
	destroyAllegro(allPtrs);

	// #################################################


}

// si devuelve 0 hay error
int parseCallback(char* key, char* value, void* userData)
{
	dataInput* myData = userData;
	if (key == NULL)
		return 0;
	if (key == "-w")
	{
		myData->width = atof(value);
	}
	else if (key == "-h")
	{
		myData->height = atof(value);
	}
	else if (key == "r")
	{
		myData->robots = atof(value);
	}
	else
	{
		return 0;
	}
	return 1;
}

Floor* createFloor(int width, int height)
{
	Floor* piso = malloc(sizeof(Floor));
	if (piso != NULL)
	{
		piso->width = width;
		piso->height = height;
		piso->tiles = malloc(sizeof(bool) * width * height);
		if (piso->tiles != NULL)
		{
			for (int i = 0; i < (width * height); i++)
			{
				piso->tiles[i] = DIRTY;
			} // ensucio
		}
		else
		{
			free(piso);
			piso = NULL;
		}
	}
	return piso;
}

Robot* createRobots(int width, int height, int n)
{
	Robot* robots = malloc(n * sizeof(Robot));
	if (robots != NULL)
	{
		for (int i = 0; i < n; i++)
		{
			robots[i].x = rand() % width + (rand() % 100) * 0.01; // 2 decimales
			robots[i].y = rand() % height + (rand() % 100) * 0.01;
		}
	}
	else
	{
		free(robots);
		robots = NULL;
	}
	return robots;
}
void deleteRobots(Robot* robots)
{
	free(robots);
}

void deleteFloor(Floor* f)
{
	if (f)
	{
		free(f->tiles);
		free(f);
	}
}

Sim* createSim(int width, int height, int robotCount)
{
	srand(time(NULL));

	Sim* simu = malloc(sizeof(Sim));
	if (simu)
	{
		simu->f = createFloor(width, height);
		simu->robs = createRobots(width, height, robotCount);
		simu->tickCount = 0;
		simu->robotCount = robotCount;
	}
	return simu;
}
void deleteSim(Sim* s)
{
	deleteFloor(s->f);
	deleteRobots(s->robs);
	free(s);
}
int simulate(Sim* s)
{
	while (chkWin(s->f) == 0)
	{
		for (int i = 0; i < s->robotCount; i++)
		{
			onTick(s->robs + i * sizeof(Robot), s->f);
		}
		s->tickCount++;
	}
	return s->tickCount;
}

void onTick(Robot* r, Floor* f)
{
	int numberOfTile = f->width * (int)r->y + (int)r->x;

	printf("ROBOT POS, X = %f, Y = %f\n", r->x, r->y);
	// NormaLizedWidht should
	//always be 1					
	f->tiles[numberOfTile] = CLEAN;
	printf("HE LIMPIADO LA X = %d, Y = %d\n", (int)r->x, (int)r->y);
	r->angle = rand() % 365 + (rand() % 100) * 0.01;
	double x = cos(r->angle * PI / 180.0) + r->x;   // creo que suma en una unidad ya
	double y = sin(r->angle * PI / 180.0) + r->y;

	if (inMatrix(f, x, y))
	{
		r->x = x;
		r->y = y;
	}
}

bool inMatrix(Floor* f, double x, double y)
{
	bool onFloor = 0;
	if (x >= 0 && x < f->width &&
		y >= 0 && y < f->height)
		onFloor = 1;
	return onFloor;
}

bool chkWin(Floor* f)
{
	bool win = 1;
	for (int i = 0; (i < f->height * f->width) && win; i++)
	{
		if (f->tiles[i] == DIRTY)
		{
			win = 0;
		}
	}
	return win;
}



AllegroTypes * initAllegro(void)
{
	AllegroTypes * myPtrs = malloc(sizeof(AllegroTypes));   // Devolver error si falla malloc
	myPtrs->display = NULL;                                 // EL WARNING ES POR NO VERIFICAR EL MALLOC = NULL
	myPtrs->event_queue = NULL;
	myPtrs->timer = NULL;


	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		//return -1;
	}

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "failed to initialize primitives!\n");
        //return -1;
    }

	myPtrs->timer = al_create_timer(1.0 / FPS); //crea el timer pero NO empieza a correr
	if (!myPtrs->timer) {
		fprintf(stderr, "failed to create timer!\n");
		al_shutdown_primitives_addon();
		//return -1;
	}

	myPtrs->event_queue = al_create_event_queue();
	if (!myPtrs->event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_shutdown_primitives_addon();
		al_destroy_timer(myPtrs->timer);
		//return -1;
	}

	myPtrs->display = al_create_display(D_WIDTH, D_HEIGHT);
	if (!myPtrs->display) {
		fprintf(stderr, "failed to create display!\n");
		al_shutdown_primitives_addon();
		al_destroy_event_queue(myPtrs->event_queue);
		al_destroy_timer(myPtrs->timer);
		//return -1;
	}

	al_register_event_source(myPtrs->event_queue, al_get_display_event_source(myPtrs->display));
	al_register_event_source(myPtrs->event_queue, al_get_timer_event_source(myPtrs->timer));
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	al_start_timer(myPtrs->timer); //Recien aca EMPIEZA el timer

	return myPtrs;
}

void destroyAllegro(AllegroTypes * myPtrs)
{
	al_shutdown_primitives_addon();
	al_destroy_timer(myPtrs->timer);
	al_destroy_display(myPtrs->display);
	al_destroy_event_queue(myPtrs->event_queue);
}

void play(Sim* s)
{
	if (chkWin(s->f) == 0)
	{
		for (int i = 0; i < s->robotCount; i++)
		{
			onTick(s->robs + i , s->f);
		}
		s->tickCount++;
	}
}

void drawSim(Sim* s)
{
	al_clear_to_color(al_color_name("black") );

	for (int i = 0; i < s->f->height; i++)
	{
		for (int j = 0; j < s->f->width; j++)
		{
			double x1 = (double)j;
			double y1 = (double)i + 1;
			double x2 = (double)j + 1;
			double y2 = (double)i;

			
			back2Front(s, &x1, &y1);   // cambiar que el parametro sea width no s <- no estoy seguro ahora, despues veer
			back2Front(s, &x2, &y2);

			if (s->f->tiles[i*s->f->width + j] == CLEAN)
			{
				al_draw_filled_rectangle(x1, y1, x2, y2, al_color_name("red"));
			}
			else if (s->f->tiles[i * s->f->width + j] == DIRTY)
			{
				al_draw_filled_rectangle(x1, y1, x2, y2, al_color_name("hotpink"));
			}
		}
	}
	for (int i = 0; i < D_HEIGHT; i += 50)
	{
		al_draw_line(0, i, D_WIDTH, i, al_color_name("black"), 1.0);
	}
	for (int j = 0; j < D_WIDTH; j += 50)
	{
		al_draw_line(j, 0, j, D_HEIGHT, al_color_name("black"), 1.0);
	}

	al_flip_display();
}

void back2Front(Sim *s, double * x, double * y)      
{
	*y = (s->f->height - *y) * TILE_SIZE;
	*x = *x * TILE_SIZE;
}