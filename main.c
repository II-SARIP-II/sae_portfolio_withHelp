#include <stdio.h>
#include <SDL3/SDL.h>
#include <direct.h>
#include <time.h>
#include <stdlib.h>
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

    // Charger une image de texte (crée une image de texte avant dans ton projet)
    SDL_Surface* surfaceMessage = SDL_LoadBMP("textLevel.bmp");  // Image pré-générée
    if (!surfaceMessage) {
        printf("Erreur de chargement de l'image de texte: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Event event;
    int running = 1;
    int difficulty = 0;

    // Écran de sélection de difficulté
    while (running) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, Message, NULL, NULL);  // Afficher la texture de texte
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
                break;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_1 ) {
                    difficulty = 10;
                    running = 0;
                    break;
                } else if (event.key.scancode == SDL_SCANCODE_2) {
                    difficulty = 20;
                    running = 0;
                    break;
                } else if (event.key.scancode == SDL_SCANCODE_3 ) {
                    difficulty = 35;
                    running = 0;
                    break;
                } else if (event.key.scancode == SDL_SCANCODE_4 ) {
                    difficulty = 50;
                    running = 0;
                    break;
                } else if (event.key.scancode == SDL_SCANCODE_5 ) {
                    difficulty = 100;
                    running = 0;
                    break;
                }
            }
        }
    }

    printf("%d ---------------------\n", difficulty);



    int gameRunning = 1;
    int result = 0;
    initGame(difficulty);

    while (gameRunning) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                gameRunning = 0;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                int x = getCaracX(difficulty);
                int y = getCaracY(difficulty);
                if (event.key.scancode == SDL_SCANCODE_UP) result = moveCarac(x, y, 'z', difficulty);
                if (event.key.scancode == SDL_SCANCODE_DOWN) result = moveCarac(x, y, 's', difficulty);
                if (event.key.scancode == SDL_SCANCODE_LEFT) result = moveCarac(x, y, 'q', difficulty);
                if (event.key.scancode == SDL_SCANCODE_RIGHT) result = moveCarac(x, y, 'd', difficulty);
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
                freeMap(difficulty);
                return 1;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
            SDL_DestroySurface(image);
            if (!texture) {
                printf("Erreur : Impossible de charger texture - %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                freeMap(difficulty);
                return 1;
            }

            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(4000);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            freeMap(difficulty);
            return 0;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        printMap(renderer, difficulty);
        SDL_RenderPresent(renderer);
    }
    freeMap(difficulty);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
