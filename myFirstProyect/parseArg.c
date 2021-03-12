/***************************************************************************//**
  @file     parseArg.c
  @author   Grupo7
 ******************************************************************************/

 /*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
 
#include <stdio.h>
#include <string.h>
#include "parseArg.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum{OK = 0, ERROR_1, ERROR_2, ERROR_CALLBACK};
enum{STATE_0, STATE_1};

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

int parseCmdLine(int argc, char * argv[], pCallback p, void * userData)
{
    int errorState = OK;
    int state = STATE_0;
    int dataCounter = 0;
    for (int i = 1; i < argc && errorState == OK ; i++)
    {
        switch (state)
        {
        case STATE_0:
            if(argv[i][0] == '-')
            {
               if(argv[i][1] == '\0')
               {
                   errorState = ERROR_1; 
                   printf("ERROR 1\n");
               }
               else
               {
                   state = STATE_1;
               }
            }
            else
            {
                if(p( NULL, argv[i], userData) == 0)
                {
                    errorState = ERROR_CALLBACK;
                    printf("ERROR CALLBACK\n");
                }
                else
                {
                    dataCounter++;
                }
            }
            break;

        case STATE_1:     // tomo el valor de la opcion
            state = STATE_0;
            if(p(argv[i - 1], argv[i], userData) == 0 )
            {
                errorState = ERROR_CALLBACK;
                printf("ERROR CALLBACK\n");
            }
            else
            {
                dataCounter++;
            }
            break;
        }
    }
    if(state == STATE_1)
    {
        errorState = ERROR_2;
        printf("ERROR 2\n");
    }
    return (errorState == 0 ? dataCounter : -1);
}