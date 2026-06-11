#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#define LARGEUR_FENETRE 600
#define HAUTEUR_FENETRE 600

typedef struct {
    SDL_Rect rect;
    const char* texte;
    SDL_Texture* texture;
    SDL_Color couleur;
} Bouton;

typedef enum {
    ETAT_MENU,
    CONTRE_IA,
    JOUEUR_VS_JOUEUR
} EtatJeu;

void reinitJeu(int* piecesJ1, int* piecesJ2);
void initMenu(SDL_Renderer* renderer, TTF_Font* font);
void dessinerMenu(SDL_Renderer* renderer);
void gererClicMenu(int x, int y, EtatJeu* etatCourante);
void nettoyerMenu();
void dessinerBouton(SDL_Renderer* renderer, Bouton* bouton);

#endif