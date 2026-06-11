#ifndef AI_H
#define AI_H

#define J1 1
#define J2 2

#include <stdbool.h>

typedef struct {
    int l;
    int col;
    int deL; 
    int deCol;
} Deplacement;

int minimax(int table[3][3], int profondeur, bool isMaximizing, int phase);
int evaluatetable(int table[3][3]);
Deplacement trouverMeuilleurDeplace(int table[3][3], int phase);
void deplaceIA(int* piecesJ1, int* piecesJ2);

#endif