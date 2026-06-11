#include "ai.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#define INFINI INT_MAX

extern bool deplacement(int deb, int fin);

int evaluatetable(int table[3][3]) {
    for (int l = 0; l < 3; l++) {
        if (table[l][0] == table[l][1] && table[l][1] == table[l][2]) {
            if (table[l][0] == J2) return 10;
            else if (table[l][0] == J1) return -10; 
        }
    }
    for (int col = 0; col < 3; col++) {
        if (table[0][col] == table[1][col] && table[1][col] == table[2][col]) {
            if (table[0][col] == J2) return 10;
            else if (table[0][col] == J1) return -10;
        }
    }
    if (table[0][0] == table[1][1] && table[1][1] == table[2][2]) {
        if (table[0][0] == J2) return 10;
        else if (table[0][0] == J1) return -10;
    }
    if (table[0][2] == table[1][1] && table[1][1] == table[2][0]) {
        if (table[0][2] == J2) return 10;
        else if (table[0][2] == J1) return -10;
    }
    return 0;
}

int minimax(int table[3][3], int profondeur, bool isMaximizing, int phase) {
    int score = evaluatetable(table);
    if (score == 10) return score - profondeur;
    if (score == -10) return score + profondeur;
    bool isMovesLeft = false;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (table[i][j] == 0) {
                isMovesLeft = true;
                break;
            }
        }
    }
    if (!isMovesLeft) return 0;
    if (isMaximizing) {
        int meilleurScore = -INFINI;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (table[i][j] == 0) {
                    table[i][j] = J2;
                    int scoreCourant = minimax(table, profondeur + 1, false, phase);
                    table[i][j] = 0;
                    meilleurScore = (scoreCourant > meilleurScore) ? scoreCourant : meilleurScore;
                }
            }
        }
        return meilleurScore;
    }
    else {
        int meilleurScore = +INFINI;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (table[i][j] == 0) {
                    table[i][j] = J1;
                    int scoreCourant = minimax(table, profondeur + 1, true, phase);
                    table[i][j] = 0;
                    meilleurScore = (scoreCourant < meilleurScore) ? scoreCourant : meilleurScore;
                }
            }
        }
        return meilleurScore;
    }
}

Deplacement trouverMeuilleurDeplace(int table[3][3], int phase) {
    Deplacement meilleurdeplace = {-1, -1, -1, -1};
    int meilleurScore = -INFINI;
    if (phase == 1) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (table[i][j] == 0) {
                    table[i][j] = J2;
                    int scoreCourant = minimax(table, 0, false, phase);
                    table[i][j] = 0;
                    if (scoreCourant > meilleurScore) {
                        meilleurScore = scoreCourant;
                        meilleurdeplace.l = i;
                        meilleurdeplace.col = j;
                    }
                }
            }
        }
    } else if (phase == 2) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (table[i][j] == J2) {
                    for (int x = 0; x < 3; x++) {
                        for (int y = 0; y < 3; y++) {
                            if (table[x][y] == 0 && deplacement(i * 3 + j + 1, x * 3 + y + 1)) {
                                table[x][y] = J2; 
                                table[i][j] = 0;
                                int scoreCourant = minimax(table, 0, false, phase);
                                table[x][y] = 0;
                                table[i][j] = J2; 
                                if (scoreCourant > meilleurScore) {
                                    meilleurScore = scoreCourant;
                                    meilleurdeplace.deL = i;
                                    meilleurdeplace.deCol = j;
                                    meilleurdeplace.l = x;
                                    meilleurdeplace.col = y;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return meilleurdeplace;
}