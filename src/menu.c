#include "menu.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

Bouton boutJouerContreIA = {{200, 200, 200, 50}, "Jouer contre IA", NULL, {0, 255, 0, 255}};
Bouton boutJoueurVsJoueur = {{200, 300, 200, 50}, "J1 vs J2", NULL, {0, 0, 255, 255}};
Bouton boutQuitter = {{200, 400, 200, 50}, "Quitter", NULL, {255, 0, 0, 255}};

SDL_Texture* textureFond = NULL;

void initMenu(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* surfaceFond = SDL_LoadBMP("src/bgm.bmp");
    if (surfaceFond) {
        textureFond = SDL_CreateTextureFromSurface(renderer, surfaceFond);
        SDL_FreeSurface(surfaceFond);
        if (!textureFond) {
            printf("Erreur lors de la création de la texture de fond: %s\n", SDL_GetError());
        }
    } else {
        printf("Erreur lors du chargement de l'image de fond: %s\n", SDL_GetError());
    }
    SDL_Color textCouleur = {0, 0, 0, 255}; 
    SDL_Surface* surfaceIA = TTF_RenderText_Solid(font, boutJouerContreIA.texte, textCouleur);
    boutJouerContreIA.texture = SDL_CreateTextureFromSurface(renderer, surfaceIA);
    SDL_FreeSurface(surfaceIA);
    SDL_Surface* surfaceJoueur = TTF_RenderText_Solid(font, boutJoueurVsJoueur.texte, textCouleur);
    boutJoueurVsJoueur.texture = SDL_CreateTextureFromSurface(renderer, surfaceJoueur);
    SDL_FreeSurface(surfaceJoueur);
    SDL_Surface* surfaceQuitter = TTF_RenderText_Solid(font, boutQuitter.texte, textCouleur);
    boutQuitter.texture = SDL_CreateTextureFromSurface(renderer, surfaceQuitter);
    SDL_FreeSurface(surfaceQuitter);
}

void dessinerMenu(SDL_Renderer* renderer) {
    if (textureFond) {
        SDL_RenderCopy(renderer, textureFond, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
    dessinerBouton(renderer, &boutJouerContreIA);
    dessinerBouton(renderer, &boutJoueurVsJoueur);
    dessinerBouton(renderer, &boutQuitter);
}

void gererClicMenu(int x, int y, EtatJeu* etatCourante) {
    if (x >= boutJouerContreIA.rect.x && x <= boutJouerContreIA.rect.x + boutJouerContreIA.rect.w &&
        y >= boutJouerContreIA.rect.y && y <= boutJouerContreIA.rect.y + boutJouerContreIA.rect.h) {
        *etatCourante = CONTRE_IA; 
    }
    else if (x >= boutJoueurVsJoueur.rect.x && x <= boutJoueurVsJoueur.rect.x + boutJoueurVsJoueur.rect.w && y >= boutJoueurVsJoueur.rect.y && y <= boutJoueurVsJoueur.rect.y + boutJoueurVsJoueur.rect.h) {
        *etatCourante = JOUEUR_VS_JOUEUR;
    }
    else if (x >= boutQuitter.rect.x && x <= boutQuitter.rect.x + boutQuitter.rect.w & y >= boutQuitter.rect.y && y <= boutQuitter.rect.y + boutQuitter.rect.h) {
        exit(0);
    }
}

void nettoyerMenu() {
    SDL_DestroyTexture(boutJouerContreIA.texture);
    SDL_DestroyTexture(boutJoueurVsJoueur.texture);
    SDL_DestroyTexture(boutQuitter.texture);
    if (textureFond) {
        SDL_DestroyTexture(textureFond);
    }
}