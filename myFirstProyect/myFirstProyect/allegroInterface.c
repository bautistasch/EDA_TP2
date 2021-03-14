#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "parseArg.h"


#include <allegro5/allegro.h>      
#include <allegro5/allegro_color.h> 
#include <allegro5/allegro_primitives.h> 

#define D_WIDTH_GRAPHIC  1200
#define D_HEIGHT_GRAPHIC 1000


#define FPS 100

#define TILE_SIZE 10          // ESTO DEBERIA SER CALCULADO POR EL PROGRAMA

#define D_MAX_WIDTH 1600
#define D_MAX_HEIGHT 1200

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

typedef struct
{
	int width;
	int height;
}displaySize;

// ######################################
// #### PROTOTIPOS ######################
// ######################################

Floor* createFloor(int width, int height);
Robot* createRobots(int width, int height, int n);
void deleteRobots(Robot* robots);
Sim* createSim(int width, int height, int robotCount);
void deleteSim(Sim* s);
// para la simulacion de 1000, no lo use todavia
long simulate(Sim* s);
void onTick(Robot* r, Floor* f);
bool inMatrix(Floor* f, double x, double y);
bool chkWin(Floor* f);
AllegroTypes* initAllegro(int dispW, int dispH);
void destroyAllegro(AllegroTypes* myPtrs);
// cambiar nombre de play, lo que hace es jugar un tick solo
int playOneTick(Sim* s);
void drawSim(Sim* s);
// tranforma coordenadas de back a front
void back2Front(Sim* s, double* x, double* y);
int adaptDispSize(int width, int height, int* dispW, int* dispH);

int parseCallback(char* key, char* value, void* userData);
void drawGraphic(int n, int* ticks);

int main(int argc, char* argv [])
{
	dataInput myDataInput;                                          // VER COMO INGRESAR COMANDO POR VS
	parseCmdLine(argc, argc, parseCallback, &myDataInput );        // No lo puedo probar
	
	int dispW, dispH;
	adaptDispSize(50, 50, &dispW, &dispH);     // <----- esta funcion se deberia rehacer

	AllegroTypes* allPtrs = initAllegro(dispW, dispH); 

	Sim* mySim = createSim(50, 50, 1);  // piso de 10 x 10, 3 aspiradoras

	bool redraw = false;
	bool display_close = false;
	bool win = 0;

	long ticksAllegro = 0;

	while (!display_close && !win) {
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
			if (!win)
			{
				win = playOneTick(mySim);
				ticksAllegro = mySim->tickCount;
			}
			drawSim(mySim);
		}
	}
	deleteSim(mySim);
	destroyAllegro(allPtrs);
	

	//printf("ALLEGRO TOOK %f ticks\n", allegroPromedio);

	long double promedio = 0;
	int* ticks = malloc(sizeof(int));
	for (int r = 1; r < 20; r++)
	{
		promedio = 0;
		for (int i = 0; i < MAXSIM; i++)
		{
			Sim* sim = createSim(50, 50, r);
			promedio += (long double)simulate(sim) / MAXSIM;
			deleteSim(sim);
		}
		printf("%d\n", r);
		ticks = realloc(ticks, r * sizeof(int));
		ticks[r - 1] = (int)promedio;
		printf("SIMULATION AVERAGE TOOK %d ticks\n",(int) promedio);
	}
	AllegroTypes * allGraphics = initAllegro(D_WIDTH_GRAPHIC, D_HEIGHT_GRAPHIC);
	drawGraphic(50, ticks);
	getchar();
	free(ticks);
	destroyAllegro(allGraphics);


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
long simulate(Sim* s)
{
	while (chkWin(s->f) == 0)
	{
		for (int i = 0; i < s->robotCount; i++)
		{
			onTick(s->robs + i , s->f);
		}
		s->tickCount++;
	}
	return s->tickCount;
}

void onTick(Robot* r, Floor* f)
{
	int numberOfTile = f->width * (int)r->y + (int)r->x;
	
	f->tiles[numberOfTile] = CLEAN;
	
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



AllegroTypes * initAllegro(int dispW, int dispH)
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

	myPtrs->display = al_create_display(dispW, dispH);
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
	free(myPtrs);
}

int playOneTick(Sim* s)
{
	if (chkWin(s->f) == 0)
	{
		for (int i = 0; i < s->robotCount; i++)
		{
			onTick(s->robs + i , s->f);
		}
		s->tickCount++;
		return 0;
	}
	return 1;
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
	for (int i = 0; i < (TILE_SIZE * s->f->height); i += TILE_SIZE)
	{
		al_draw_line(0, i, (TILE_SIZE * s->f->width), i, al_color_name("black"), 1.0);
	}
	for (int j = 0; j < (TILE_SIZE * s->f->width); j += TILE_SIZE)
	{
		al_draw_line(j, 0, j, (TILE_SIZE * s->f->height), al_color_name("black"), 1.0);
	}

	al_flip_display();
}



void back2Front(Sim *s, double * x, double * y)      // ver de nuevo
{
	*y = (s->f->height - *y) * TILE_SIZE;
	*x = *x * TILE_SIZE;
}

void drawGraphic(int n, int * ticks) //  n = 1 asociado con ticks[n-1]  f(n-1) = ticks[n-1]
{
	int pasoX = (D_WIDTH_GRAPHIC - 10) / n;
	int maxY = *ticks; // el primer elemento deberia ser el mas grande

	for (int j = 1; j <= n; j++ )
	{
		al_draw_filled_circle(j * pasoX, D_HEIGHT_GRAPHIC - (D_HEIGHT_GRAPHIC - 10) * ticks[j - 1] / maxY, 5, al_color_name("red"));
	}
	
	al_flip_display();
}

int adaptDispSize(int width, int height, int* dispW, int* dispH)
{
	if ((width * TILE_SIZE <= D_MAX_WIDTH) && (height * TILE_SIZE <= D_MAX_HEIGHT))
	{
		*dispW = width * TILE_SIZE;
		*dispH = height * TILE_SIZE;
		return 1;
	}
	else
	{
		printf("Change TILE_SIZE, too large");
	}
	return 0;
}
