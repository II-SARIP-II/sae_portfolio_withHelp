#include <stdio.h>
#include <SDL3/SDL.h>
#include <direct.h>
#include <time.h>
#include <stdlib.h>
#include "solveur.h"
#include "game.h"


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Maze Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int running = 0;
    SDL_Event event;
    int result = 0;
    initGame();

    while (running != 1) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 1;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                int x = getCaracX();
                int y = getCaracY();
                if (event.key.scancode == SDL_SCANCODE_UP) result = moveCarac(x, y, 'z');
                if (event.key.scancode == SDL_SCANCODE_DOWN) result = moveCarac(x, y, 's');
                if (event.key.scancode == SDL_SCANCODE_LEFT) result = moveCarac(x, y, 'q');
                if (event.key.scancode == SDL_SCANCODE_RIGHT) result = moveCarac(x, y, 'd');
            }

        }
        if (result == 1){
            printf("WIIIIIIIIIIIIIIIIN");
            SDL_Surface *image = SDL_LoadBMP("victory.bmp");
            if (!image) {
                printf("Erreur chargement image: %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
            SDL_DestroySurface(image);
            if (!texture) {
                printf("Erreur : Impossible de charger texture - %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_Delay(4000);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();

            return 0;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        printMap(renderer);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
