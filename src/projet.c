// cd Desktop\jeuachi 
// gcc src/projet.c src/menu.c src/ai.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
// .\bin\prog
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "menu.h"
#include "ai.h"
#include <limits.h>

#define INFINI INT_MAX


#define LARGEUR_FENETRE 600
#define HAUTEUR_FENETRE 600
#define TAILLE_CASE (LARGEUR_FENETRE/3)

typedef struct {
    Uint8 r,v,b,a;
}Couleur;

Couleur COULEUR_TABLE = {0, 0, 0, 255};        
Couleur COULEUR_J1 = {255, 0, 0, 255};   
Couleur COULEUR_J2 = {0, 0, 255, 255};   
Couleur COULEUR_FOND = {173, 216, 230, 255}; 
Couleur COULEUR_BORDURE = {255, 255, 0, 255};  
Couleur COULEUR_TEXTE = {255, 255, 255, 255};     
Couleur COULEUR_BANNIERE = {50, 50, 50, 255};

Bouton BoutonRejouer = {{150, 370, 100, 40}, "Rejouer", NULL, {0, 255, 0, 255}}; 
Bouton BoutonRetourner = {{350, 370, 100, 40}, "Retourner", NULL, {255, 0, 0, 255}};   


bool estAdjacent(int l1, int col1, int l2, int col2);

int table[3][3] = {{0}}; 
int tour = 1;   
int phase = 1;  
int pieceSelectionee[2] = {-1, -1};

int init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return 0;
    }
    if (TTF_Init() == -1) { 
        printf("Erreur lors de l'initialisation de SDL_ttf: %s\n", TTF_GetError());
        return 0;
    }
    *window = SDL_CreateWindow("ACHI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR_FENETRE, HAUTEUR_FENETRE, SDL_WINDOW_SHOWN);
    if (!*window) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        return 0;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        printf("Erreur lors de la création du rendu: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

void dessinerTable(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, COULEUR_TABLE.r, COULEUR_TABLE.v, COULEUR_TABLE.b, COULEUR_TABLE.a);
    int epaiss = 5;
    for (int i = -epaiss / 2; i <= epaiss / 2; i++) {
        SDL_RenderDrawLine(renderer, (LARGEUR_FENETRE / 2) + i, 0, (LARGEUR_FENETRE / 2) + i, HAUTEUR_FENETRE);
        SDL_RenderDrawLine(renderer, 0, (HAUTEUR_FENETRE / 2) + i, LARGEUR_FENETRE, (HAUTEUR_FENETRE / 2) + i); 
    }
    for (int i = -epaiss / 2; i <= epaiss / 2; i++) {
        SDL_RenderDrawLine(renderer, 0 + i, 0, LARGEUR_FENETRE + i, HAUTEUR_FENETRE); 
        SDL_RenderDrawLine(renderer, LARGEUR_FENETRE + i, 0, 0 + i, HAUTEUR_FENETRE);
    }
}

void dessinerCircle(SDL_Renderer* renderer, int centreX, int centreY, int rayon) {
    for (int w = 0; w < rayon * 2; w++) {
        for (int h = 0; h < rayon * 2; h++) {
            int dx = rayon - w;
            int dy = rayon - h;
            if ((dx * dx + dy * dy) <= (rayon * rayon)) {
                SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy);
            }
        }
    }
}

void dessinerBordure(SDL_Renderer* renderer, int centreX, int centreY, int rayon) {
    int x = rayon;
    int y = 0;
    int p = 1 - rayon;
    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        y++;
        if (p <= 0) {
            p += 2 * y + 1;
        } else {
            x--;
            p += 2 * (y - x) + 1;
        }
    }
}

void dessinerPieces(SDL_Renderer* renderer) {
    for (int l = 0; l < 3; l++) {
        for (int col = 0; col < 3; col++) {
            if (table[l][col] != 0) {
                Couleur Couleur = (table[l][col] == 1) ? COULEUR_J1 : COULEUR_J2;
                SDL_SetRenderDrawColor(renderer, Couleur.r, Couleur.v, Couleur.b, Couleur.a);
                int centreX = col * TAILLE_CASE + TAILLE_CASE / 2;
                int centreY = l * TAILLE_CASE + TAILLE_CASE / 2;
                int rayon = TAILLE_CASE / 6;
                dessinerCircle(renderer, centreX, centreY, rayon);
                if (pieceSelectionee[0] == l && pieceSelectionee[1] == col) {
                    SDL_SetRenderDrawColor(renderer, COULEUR_BORDURE.r, COULEUR_BORDURE.v, COULEUR_BORDURE.b, COULEUR_BORDURE.a);
                    dessinerBordure(renderer, centreX, centreY, rayon + 10);
                }
            }
        }
    }
}

bool deplacement(int deb, int fin) {
    const int deplaceValide[9][9] = {
        {0, 1, 0, 1, 1, 0, 0, 0, 0}, 
        {1, 0, 1, 0, 1, 0, 0, 0, 0}, 
        {0, 1, 0, 0, 1, 1, 0, 0, 0}, 
        {1, 0, 0, 0, 1, 0, 1, 0, 0}, 
        {1, 1, 1, 1, 0, 1, 1, 1, 1}, 
        {0, 0, 1, 0, 1, 0, 0, 0, 1}, 
        {0, 0, 0, 1, 1, 0, 0, 1, 0}, 
        {0, 0, 0, 0, 1, 0, 1, 0, 1}, 
        {0, 0, 0, 0, 1, 1, 0, 1, 0}  
    };
    return deplaceValide[deb-1][fin-1] == 1;
}

void afficherGagnant(SDL_Renderer* renderer, int gagnant, EtatJeu etatCourante) {
    SDL_Rect banniere = {100, 250, 400, 100};
    SDL_SetRenderDrawColor(renderer, COULEUR_BANNIERE.r, COULEUR_BANNIERE.v, COULEUR_BANNIERE.b, COULEUR_BANNIERE.a);
    SDL_RenderFillRect(renderer, &banniere);
    TTF_Font* font = TTF_OpenFont("src/ARCADECLASSIC.ttf", 36);
    if (!font) {
        printf("Erreur lors du chargement de la police: %s\n", TTF_GetError());
        return;
    }
    char message[50];
    if (gagnant == J1) {
        sprintf(message, "Joueur 1 a gagne !");
    } else if (gagnant == J2) {
        if (etatCourante == CONTRE_IA) {
            sprintf(message, "IA a gagne !");
        } else {
            sprintf(message, "Joueur 2 a gagne !");
        }
    }
    SDL_Color couleurTexte = {255, 255, 255, 255};
    SDL_Surface* surfaceTexte = TTF_RenderText_Solid(font, message, couleurTexte);
    if (!surfaceTexte) {
        printf("Erreur lors de la création de la surface de texte: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }
    SDL_Texture* textureTexte = SDL_CreateTextureFromSurface(renderer,surfaceTexte);
    SDL_FreeSurface(surfaceTexte);
    if (!textureTexte) {
        printf("Erreur lors de la création de la texture de texte: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        return;
    }
    int LargeurTexte = 0, LongeurTexte = 0;
    SDL_QueryTexture(textureTexte, NULL, NULL, &LargeurTexte, &LongeurTexte);
    SDL_Rect rectTexte = {banniere.x + (banniere.w - LargeurTexte) / 2, banniere.y + (banniere.h - LongeurTexte) / 2, LargeurTexte, LongeurTexte};
    SDL_RenderCopy(renderer, textureTexte, NULL, &rectTexte);
    SDL_DestroyTexture(textureTexte);
    TTF_CloseFont(font);
}

int detectVict() {
    for (int i = 0; i < 3; i++) {
        if (table[i][0] != 0 && table[i][0] == table[i][1] && table[i][1] == table[i][2]) return table[i][0];
        if (table[0][i] != 0 && table[0][i] == table[1][i] && table[1][i] == table[2][i]) return table[0][i]; 
    }
    if (table[0][0] != 0 && table[0][0] == table[1][1] && table[1][1] == table[2][2]) return table[0][0]; 
    if (table[0][2] != 0 && table[0][2] == table[1][1] && table[1][1] == table[2][0]) return table[0][2]; 
    return 0;
}

void gererClicJeu(int x, int y, EtatJeu* etatCourante, int* gagnant, int* piecesJ1, int* piecesJ2) {
    int col = x / TAILLE_CASE;
    int l = y / TAILLE_CASE;
    if (*gagnant != 0) {
        if (x >= BoutonRejouer.rect.x && x <= BoutonRejouer.rect.x + BoutonRejouer.rect.w && y >= BoutonRejouer.rect.y && y <= BoutonRejouer.rect.y + BoutonRejouer.rect.h) {
            reinitJeu(piecesJ1, piecesJ2);
            *gagnant = 0;
        }
        else if (x >= BoutonRetourner.rect.x && x <= BoutonRetourner.rect.x + BoutonRetourner.rect.w && y >= BoutonRetourner.rect.y && y <= BoutonRetourner.rect.y + BoutonRetourner.rect.h) {
            *etatCourante = ETAT_MENU;
            reinitJeu(piecesJ1, piecesJ2);
            *gagnant = 0;
        }
        return;
    }
    if (l >= 0 && l < 3 && col >= 0 && col < 3) {
        if (phase == 1) {
            if (table[l][col] == 0) {
                if (tour == J1 && *piecesJ1 < 3) {
            table[l][col] = J1;
            (*piecesJ1)++;
            printf("Joueur 1 place une piece en (%d,%d)\n", l, col);
        } else if (tour == J2 && *piecesJ2 < 3) {
            table[l][col] = J2;
            (*piecesJ2)++;
            if (*etatCourante == CONTRE_IA) {
                printf("IA place une piece en (%d,%d)\n", l, col); 
            } else {
                printf("Joueur 2 place une piece en (%d,%d)\n", l, col); 
            }
        }
        if (*piecesJ1 == 3 && *piecesJ2 == 3) {
            phase = 2;
            printf("Toutes les pieces sont places. Passage a la phase de deplacement.\n");
        }
        tour = (tour == J1) ? J2 : J1;
    }
} else if (phase == 2) {
    if (pieceSelectionee[0] == -1) {
        if (table[l][col] == tour) {
            pieceSelectionee[0] = l;
            pieceSelectionee[1] = col;
            if (*etatCourante == CONTRE_IA && tour == J2) {
                printf("IA selectionne la piece en (%d,%d)\n", l, col);
            } else {
                printf("Joueur %d selectionne la piece en (%d,%d)\n", tour, l, col);
            }
        }
    } else {
        if (l == pieceSelectionee[0] && col == pieceSelectionee[1]) {
            pieceSelectionee[0] = -1;
            pieceSelectionee[1] = -1;
            if (*etatCourante == CONTRE_IA && tour == J2) {
                printf("IA annule la selection de la piece en (%d,%d)\n", l, col); 
            } else {
                printf("Joueur %d annule la selection de la piece en (%d,%d)\n", tour, l, col);
            }
        }
        else if (table[l][col] == tour) {
            pieceSelectionee[0] = l;
            pieceSelectionee[1] = col;
            if (*etatCourante == CONTRE_IA && tour == J2) {
                printf("IA selectionne une nouvelle piece en (%d,%d)\n", l, col); 
            } else {
                printf("Joueur %d selectionne une nouvelle piece en (%d,%d)\n", tour, l, col); 
            }
        }
        else {
            int deb = pieceSelectionee[0] * 3 + pieceSelectionee[1] + 1;
            int fin= l * 3 + col + 1;
            if (table[l][col] == 0 && deplacement(deb, fin)) {
                table[l][col] = tour;
                table[pieceSelectionee[0]][pieceSelectionee[1]] = 0;
                if (*etatCourante == CONTRE_IA && tour == J2) {
                    printf("IA deplace une piece de (%d,%d) a (%d,%d)\n",
                        pieceSelectionee[0], pieceSelectionee[1], l, col);
                } else {
                    printf("Joueur %d deplace une piece de (%d,%d) a (%d,%d)\n",
                        tour, pieceSelectionee[0], pieceSelectionee[1], l, col); 
                }
                pieceSelectionee[0] = -1;
                pieceSelectionee[1] = -1;
                tour = (tour == J1) ? J2 : J1;
            }
        }
    }
}
}
}

void deplaceIA(int* piecesJ1, int* piecesJ2) {
    if (tour == J2) {
        Deplacement meuilleurDeplace = trouverMeuilleurDeplace(table, phase);
        if (phase == 1 && *piecesJ2 < 3) {
            if (meuilleurDeplace.l != -1 && meuilleurDeplace.col != -1) {
                table[meuilleurDeplace.l][meuilleurDeplace.col] = J2;
                (*piecesJ2)++;
                printf("IA place une piece en (%d,%d)\n", meuilleurDeplace.l, meuilleurDeplace.col);
                if (*piecesJ1 == 3 && *piecesJ2 == 3) {
                    phase = 2;
                    printf("Toutes les pieces sont placees. Passage a la phase de deplacement.\n");
                }
            }
        } else if (phase == 2) {
            if (meuilleurDeplace.deL != -1 && meuilleurDeplace.deCol != -1 && meuilleurDeplace.l != -1 && meuilleurDeplace.col != -1) {
                table[meuilleurDeplace.deL][meuilleurDeplace.deCol] = 0;
                table[meuilleurDeplace.l][meuilleurDeplace.col] = J2;
                printf("IA deplace une piece de (%d,%d) a (%d,%d)\n",meuilleurDeplace.deL, meuilleurDeplace.deCol, meuilleurDeplace.l, meuilleurDeplace.col);
            }
        }
        tour = J1; 
    }
}

bool estAdjacent(int l1, int col1, int l2, int col2) {
    int lDiff = abs(l1 - l2);
    int colDiff = abs(col1 - col2);
    return (lDiff <= 1 && colDiff <= 1 && !(lDiff == 0 && colDiff == 0));
}


SDL_Texture* chargerFond(SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadBMP("src/bgj.bmp");
    if (!surface) {
        printf("Erreur de chargement du fond: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void creerTexturesBoutons(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color couleurTexte = {0, 0, 0, 255};
    SDL_Surface* surfaceRejouer = TTF_RenderText_Solid(font, BoutonRejouer.texte, couleurTexte);
    BoutonRejouer.texture = SDL_CreateTextureFromSurface(renderer, surfaceRejouer);
    SDL_FreeSurface(surfaceRejouer);
    SDL_Surface* surfaceFermer = TTF_RenderText_Solid(font, BoutonRetourner.texte, couleurTexte);
    BoutonRetourner.texture = SDL_CreateTextureFromSurface(renderer, surfaceFermer);
    SDL_FreeSurface(surfaceFermer);
}

void dessinerBouton(SDL_Renderer* renderer, Bouton* bouton) {
    SDL_SetRenderDrawColor(renderer, bouton->couleur.r, bouton->couleur.g, bouton->couleur.b, bouton->couleur.a);
    SDL_RenderFillRect(renderer, &bouton->rect); 
    if (bouton->texture) {
        int textL, textH;
        SDL_QueryTexture(bouton->texture, NULL, NULL, &textL, &textH);
        SDL_Rect rectTexte = {
            bouton->rect.x + (bouton->rect.w - textL) / 2,
            bouton->rect.y + (bouton->rect.h - textH) / 2,
            textL,
            textH
        };
        SDL_RenderCopy(renderer, bouton->texture, NULL, &rectTexte);
    }
}

void reinitJeu(int* piecesJ1, int* piecesJ2) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            table[i][j] = 0;
        }
    }
    tour = J1;
    phase = 1;
    pieceSelectionee[0] = -1;
    pieceSelectionee[1] = -1;
    *piecesJ1 = 0;
    *piecesJ2 = 0;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    EtatJeu etatCourante = ETAT_MENU;
    int piecesJ1 = 0; 
    int piecesJ2 = 0;
    if (!init(&window, &renderer)) return -1;
    TTF_Font* font = TTF_OpenFont("src/ARCADECLASSIC.ttf", 20);
    if (!font) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        return -1;
    }
    initMenu(renderer, font);
    SDL_Texture* backgroundTexture = chargerFond(renderer);
    SDL_Color couleurTexte = {0, 0, 0, 255};
    SDL_Surface* surfaceRejouer = TTF_RenderText_Solid(font, BoutonRejouer.texte, couleurTexte);
    BoutonRejouer.texture = SDL_CreateTextureFromSurface(renderer, surfaceRejouer);
    SDL_FreeSurface(surfaceRejouer);
    SDL_Surface* returnSurface = TTF_RenderText_Solid(font, BoutonRetourner.texte, couleurTexte);
    BoutonRetourner.texture = SDL_CreateTextureFromSurface(renderer, returnSurface);
    SDL_FreeSurface(returnSurface);
    bool marche = true;
    SDL_Event event;
    int gagnant = 0;
    while (marche) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                marche = false;
            } 
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int sourisX = event.button.x;
                int sourisY = event.button.y;
                switch(etatCourante) {
                    case ETAT_MENU:
                        gererClicMenu(sourisX, sourisY, &etatCourante);
                        if (etatCourante != ETAT_MENU) {
                            reinitJeu(&piecesJ1, &piecesJ2);
                            gagnant = 0;
                        }
                        break;
                    case JOUEUR_VS_JOUEUR:
                    case CONTRE_IA:
                        gererClicJeu(sourisX, sourisY, &etatCourante, &gagnant, &piecesJ1, &piecesJ2);
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, COULEUR_FOND.r, COULEUR_FOND.v, COULEUR_FOND.b, COULEUR_FOND.a);
        SDL_RenderClear(renderer);
        switch(etatCourante) {
            case ETAT_MENU:
                dessinerMenu(renderer);
                break;      
            case JOUEUR_VS_JOUEUR:
            case CONTRE_IA:
                SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
                dessinerTable(renderer);
                dessinerPieces(renderer);     
                if (gagnant == 0) {
                    gagnant = detectVict();
                    if (etatCourante == CONTRE_IA && tour == 2 && gagnant == 0) {
                        deplaceIA(&piecesJ1, &piecesJ2);
                    }
                }     
                if (gagnant != 0) {
                    afficherGagnant(renderer, gagnant, etatCourante);
                    dessinerBouton(renderer, &BoutonRejouer);
                    dessinerBouton(renderer, &BoutonRetourner);
                }
                break;
        }
        SDL_RenderPresent(renderer);
    }
    nettoyerMenu();
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(BoutonRejouer.texture);
    SDL_DestroyTexture(BoutonRetourner.texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}