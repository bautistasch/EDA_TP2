#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAXSIM 1000
#define PI 3.14159265
enum {CLEAN, DIRTY};

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

Floor* createFloor(int width, int height)
{
	Floor* piso = malloc(sizeof(Floor));
	if (piso != NULL)
	{
		piso->width = width;
		piso->height = height;
		piso->tiles = malloc(sizeof(bool) * width * height);
		if(piso->tiles != NULL)
		{
			for (int i = 0; i < width * height; i++)
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

void deleteFloor(Floor * f)
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
void deleteSim(Sim * s)
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
			onTick(s->robs + i*sizeof(Robot), s->f );
		}
		s->tickCount++;
	}
	return s->tickCount;
}

int onTick(Robot * r, Floor * f)
{
	int numberOfTile =  f->width * (int)r->y + (int)r->x;
																							// NormaLizedWidht should
																						    //always be 1					
	f->tiles[numberOfTile] = CLEAN;
	r->angle = rand() % 365 + (rand() % 100) * 0.01;
	double x = cos( r->angle * PI / 180.0 ) + r->x;   // creo que suma en una unidad ya
	double y = sen(r->angle * PI / 180.0) + r->y;

	if (inMatrix(f, x, y) )
	{
		r->x = x;
		r->y = y;
	}
}

bool inMatrix(Floor * f , double x, double y)
{
	bool onFloor = 0;
	if (x >= 0 && x < f->width &&
		y >= 0 && y < f->height)
		onFloor = 1;
	return onFloor;
}

bool chkWin(Floor * f)
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

int main()
{
	/*
	parseCmdLine();
	double tickMean[1000] = 0;
	for(robotCondition())
	for (int i = 0; i < MAXSIM; i++)
	{
		Sim* s = createSim(?,r+1 );
		tickMean[r] += simulate(s);
		deleteSim(s);
	}
	drawSim(s);
	*/
	Sim* s = createSim(width, height, robotCount);
	int ticks = simulate(s);
	printf("ticks = %d\n", ticks);

}
