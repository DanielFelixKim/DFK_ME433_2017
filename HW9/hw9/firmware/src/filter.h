#ifndef _FILTER_H
#define _FILTER_H
#include <stdint.h> 

#define WINDOW_SIZE 6

int MAFfilter (int in); 
int FIRfilter (int in);
int IIRfilter (int in);
float fir_val[WINDOW_SIZE] = {.05, .15, .3, .3, .15, .05} ;
int iirBuffer[WINDOW_SIZE] = {0, 0, 0, 0, 0};
const float iirVal = .1;
int MAFBuffer[WINDOW_SIZE] ={0, 0, 0, 0, 0};
int FIRBuffer[WINDOW_SIZE] = {0, 0, 0, 0, 0};

#endif