#include "filter11.h"


int MAFfilter (int in){
    int i;
    int out;
    for(i = 0; i < WINDOW_SIZE-1; i++){
        MAFBuffer[i] = MAFBuffer[i+1];
        out += MAFBuffer[i];
    }
    MAFBuffer[i] = in;
    out += in;
    
    return (out/WINDOW_SIZE);
}

int FIRfilter (int in){
    int i=0;
    int out=0;
    for(i = 0; i < WINDOW_SIZE-1; i++){
        FIRBuffer[i] = FIRBuffer[i+1];
        out += FIRBuffer[i]*fir_val[i];
    }
    FIRBuffer[i] = in;
    out += in * fir_val[i];
    return out;
}


int IIRfilter (int in){
    int out = iirBuffer[0] * iirVal + in*(1-iirVal);
    iirBuffer[0] = out;
    return out; 
}