#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 900

//ICI C'EST LES TYPES DE CASES
#define WALL 1
//#define GHOSTWALL 4
#define EMPTY 0
#define MAINCARACTERE 2
#define FINISH 3

//ICI LA MAP DYNAMIQUE (pas trop compris)
typedef struct {
    char **map;
} terrain;

typedef struct {
    int x, y;
} Point;

//Fonctions de génération et manipulation du labyrinthe
void GenerateMaze(int difficulty);
terrain* returnMap();
void setCarac(int type,int difficulty);
int getCaracX(int difficulty);
int getCaracY(int difficulty);
int moveCarac(int x, int y, char action, int difficulty);
void printMap(SDL_Renderer *renderer, int difficulty);
void initGame(int difficulty);
void freeMap(int difficulty);

//SOLVEUR
void solveMaze(int difficulty);


//---------------------------------------- ANCIEN SOLVEUR HOMEMADE ----------------------------------------//
//
//typedef struct Case
//{
//    int px;
//    int py;
//    char action;
//}Case;
//
//typedef struct StackElement
//{
//    Case cc;
//    struct StackElement *next;
//
//}StackElement, *Stack;
//
//Stack new_stack(void);                              //CRÉATION du lifo
//int is_empty_stack(Stack st);                       // voir si c'est vide ou rempli
//void print_stack(Stack st);                         // Afficher le lifo en cours
//Stack push_stack(Stack st, Case cc, terrain *t);    //ajoute un case au lifo
//Stack pop_stack(Stack st);                          //enelve le dernier element du lifo
//void free_stack(Stack *st);                        // enleve tous les elements du lifo
//Case top_stack(Stack st);                           //Donne le dernier element (tout en haut du coup comme c'est lifo/le plus récent)
//int stack_lenght(Stack st);                         //retourne la taille
//terrain* CopyMaze(const terrain* original);
//Stack solveur(terrain *t, Stack st);                 //Logique de récursivité qui permet de trouver un chemin
//char **copy_map(char **map);
#endif // GAME_H
