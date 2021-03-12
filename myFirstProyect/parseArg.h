/***************************************************************************//**
  @file     parseArg.h
  @author   Grupo7
 ******************************************************************************/

#ifndef PARSE_ARG_H
#define PARSE_ARG_H

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef int (*pCallback) (char *, char*, void *);

/**
 * @brief Function in charge of parsing command line arguments dividing them into options (keys and values) and parameters
 * @param argc  number of words entered in the comand line
 * @param argv  an array that contains the words entered as char pointers
 * @param p the function that will be called when an option or a parameter is encountered
 * @param userData a pointer to the structure where data is going to be stored
 * @return returns -1 in case of error, and returns the sum of options and parameters in case everything is fine
*/
int parseCmdLine(int argc, char * argv[], pCallback p, void * userData);


#endif