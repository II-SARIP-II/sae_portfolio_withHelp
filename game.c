#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

terrain map;
Point *Exposed;
int exposedCount = 0;
int CountToExit = 0;
int ExitPositionWidth;

void addExposed(Point p) {
    Exposed[exposedCount++] = p;
}

Point removeExposed(int index) {
    Point p = Exposed[index];
    Exposed[index] = Exposed[--exposedCount];
    return p;
}

bool Decide(Point p, int difficulty) {
    int emptyCount = 0;
    if (p.x > 0 && map.map[p.x - 1][p.y] == EMPTY) emptyCount++;
    if (p.x < difficulty - 1 && map.map[p.x + 1][p.y] == EMPTY) emptyCount++;
    if (p.y > 0 && map.map[p.x][p.y - 1] == EMPTY) emptyCount++;
    if (p.y < difficulty - 1 && map.map[p.x][p.y + 1] == EMPTY) emptyCount++;
    return emptyCount == 1;
}

void Dig(Point p, int difficulty) {
    map.map[p.x][p.y] = EMPTY;
    Point neighbors[] = {{p.x - 1, p.y}, {p.x + 1, p.y}, {p.x, p.y - 1}, {p.x, p.y + 1}};
    for (int i = 0; i < 4; i++) {
        if (neighbors[i].x >= 0 && neighbors[i].x < difficulty && neighbors[i].y >= 0 && neighbors[i].y < difficulty) {
            if (map.map[neighbors[i].x][neighbors[i].y] == WALL) {
                addExposed(neighbors[i]);
            }
        }
    }
}

void GenerateMaze(int difficulty) {
    srand(time(NULL));

    // Allocation dynamique
    map.map = malloc(difficulty * sizeof(*map.map));
    if (!map.map) {
        perror("Erreur d'allocation pour map");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < difficulty; i++) {
        map.map[i] = malloc(difficulty * sizeof(*map.map[i]));
        if (!map.map[i]) {
            perror("Erreur d'allocation pour map[i]");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < difficulty; j++) {
            map.map[i][j] = WALL;
        }
    }

    // Allocation pour Exposed
    Exposed = malloc(difficulty * difficulty * sizeof(Point));
    if (!Exposed) {
        perror("Erreur d'allocation pour Exposed");
        exit(EXIT_FAILURE);
    }

    int entryPos = rand() % (difficulty - 2) + 1;
    map.map[0][entryPos] = EMPTY;
    Point entryPoint = {1, entryPos};
    addExposed(entryPoint);

    while (exposedCount > 0) {
        int randIndex = rand() % exposedCount;
        Point point = removeExposed(randIndex);
        if (Decide(point, difficulty)) {
            Dig(point, difficulty);
        } else {
            map.map[point.x][point.y] = WALL;
        }

        if (point.x == difficulty - 2) {
            CountToExit++;
            if (CountToExit == difficulty / 2 - 1) {
                map.map[point.x + 1][point.y] = EMPTY;
                ExitPositionWidth = point.y;
            }
        }
    }
}

void freeMap(int difficulty) {
    for (int i = 0; i < difficulty; i++) {
        free(map.map[i]);
    }
    free(map.map);
    free(Exposed);
}


terrain* returnMap(void) {
    return &map;
}

void setCarac(int type, int difficulty) {
    int y, x;
    do {
        x = rand() % (difficulty - 2) + 1;
        y = rand() % (difficulty - 2) + 1;
    } while (map.map[y][x] != EMPTY);

    if (type == 0)
        map.map[y][x] = MAINCARACTERE;
    else
        map.map[y][x] = FINISH;
}

int getCaracX(int difficulty) {
    for (int i = 0; i < difficulty; ++i)
        for (int j = 0; j < difficulty; ++j)
            if (map.map[i][j] == MAINCARACTERE)
                return j;
    return -1;
}

int getCaracY(int difficulty) {
    for (int i = 0; i < difficulty; ++i)
        for (int j = 0; j < difficulty; ++j)
            if (map.map[i][j] == MAINCARACTERE)
                return i;
    return -1;
}

int moveCarac(int x, int y, char action, int difficulty) {
    int newY = y, newX = x;
    switch (action) {
        case 'z': newY--; break;
        case 's': newY++; break;
        case 'q': newX--; break;
        case 'd': newX++; break;
    }
    if (newY >= 0 && newY < difficulty && newX >= 0 && newX < difficulty &&
        (map.map[newY][newX] == EMPTY || map.map[newY][newX] == FINISH)) {
        map.map[y][x] = EMPTY;
        if (map.map[newY][newX] == FINISH) {
            printf("YOU WIN!\n");
            return 1;
        }
        map.map[newY][newX] = MAINCARACTERE;
    }
    return 0;
}
void printMap(SDL_Renderer *renderer, int difficulty) {
    float cellWidth = (float)WINDOW_WIDTH / difficulty;
    float cellHeight = (float)WINDOW_HEIGHT / difficulty;

    for (int x = 0; x < difficulty; x++) {
        for (int y = 0; y < difficulty; y++) {
            SDL_FRect rect = {
                    x * cellWidth,
                    y * cellHeight,
                    cellWidth + 1,  // Ajout de 1 pixel pour éviter les espaces
                    cellHeight + 1
            };

            if (map.map[y][x] == WALL) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &rect);
            } else if (map.map[y][x] == MAINCARACTERE) {
                SDL_Surface *image = SDL_LoadBMP("runningPixel.bmp");
                if (!image) {
                    printf("Erreur chargement image: %s\n", SDL_GetError());
                }
                if (image) {
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
                    SDL_DestroySurface(image);
                    if (texture) {
                        SDL_FRect destRect = rect;
                        SDL_RenderTexture(renderer, texture, NULL, &destRect);
                        SDL_DestroyTexture(texture);
                    }
                }

            } else if (map.map[y][x] == FINISH) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderRect(renderer, &rect);
        }
    }
}

void initGame(int difficulty) {
    GenerateMaze(difficulty);
    setCarac(0, difficulty);
    setCarac(1, difficulty);
}

//    Stack sta = new_stack();
//    Case start = {getCaracX(),getCaracY(),'s'};
//    sta = push_stack(sta,start,&map);
//    Stack stack;
//    stack = solveur(&map, sta);
//    print_stack(stack);
//    free_stack(&stack);
//    free_stack(&sta);




/*----------------------------------------------------------------------------------------*/

/*-----------------------------------------SOLVEUR with help-----------------------------------------------*/

/*----------------------------------------------------------------------------------------*/
#define MAX_QUEUE_SIZE (WIDTH * HEIGHT)

typedef struct {
    Point point;
    int prevIndex;
} Node;

Point directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

void solveMaze(int difficulty) {
    terrain *map = returnMap();
    int startX = getCaracX(difficulty);
    int startY = getCaracY(difficulty);

    Node queue[MAX_QUEUE_SIZE];
    int visited[HEIGHT][WIDTH] = {0};
    int front = 0, rear = 0;

    queue[rear++] = (Node){{startX, startY}, -1};
    visited[startY][startX] = 1;

    int finishX = -1, finishY = -1;

    while (front < rear) {
        Node current = queue[front++];

        if (map->map[current.point.y][current.point.x] == FINISH) {
            finishX = current.point.x;
            finishY = current.point.y;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int newX = current.point.x + directions[i].x;
            int newY = current.point.y + directions[i].y;

            if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT &&
                !visited[newY][newX] &&
                (map->map[newY][newX] == EMPTY || map->map[newY][newX] == FINISH)) {

                visited[newY][newX] = 1;
                queue[rear++] = (Node){{newX, newY}, front - 1};
            }
        }
    }

    if (finishX == -1 || finishY == -1) {
        printf("No solution found!\n");
        return;
    }

    int currentIndex = rear - 1;
    char path[MAX_QUEUE_SIZE];
    int pathLength = 0;

    while (currentIndex != -1) {
        Node current = queue[currentIndex];
        Node prev = queue[queue[currentIndex].prevIndex];

        if (currentIndex != 0) {
            if (current.point.x == prev.point.x) {
                path[pathLength++] = (current.point.y < prev.point.y) ? 'z' : 's';
            } else {
                path[pathLength++] = (current.point.x < prev.point.x) ? 'q' : 'd';
            }
        }

        currentIndex = queue[currentIndex].prevIndex;
    }

    printf("Path to solve the maze: ");
    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%c ", path[i]);
    }
    printf("\nMaze solved!\n");
}



/*----------------------------------------------------------------------------------------*/

/*-----------------------------------------my own SOLVEUR (do not work with the random wall generation)-----------------------------------------------*/

/*----------------------------------------------------------------------------------------*/

//Stack new_stack(void) {
//    return NULL;
//}
//
///* ---------------------------------------------------------------------- */
//
//int is_empty_stack(Stack st) {
//    if (st == NULL)
//        return 0;
//    return 1;
//}
//
///* ---------------------------------------------------------------------- */
//
//Stack push_stack(Stack st, Case c, terrain *t) {
//    StackElement *element;
//
//    element = malloc(sizeof(*element));
//    if (element == NULL) {
//        fprintf(stderr, "PB d'allocation dynamique.\n");
//        exit(EXIT_FAILURE);
//    }
//
//    element->cc.px = c.px;
//    element->cc.py = c.py;
//    element->cc.action = c.action;
//
//    element->next = st;
//    return element;
//}
//
///* ---------------------------------------------------------------------- */
//
//void free_stack(Stack *st) {
//    while (!is_empty_stack(st)) {
//        pop_stack(st);
//    }
//}
//
///* ---------------------------------------------------------------------- */
//
//void print_stack(Stack st) {
//    if (is_empty_stack(st) == 0) {
//        printf("Stack vide.\n");
//        return;
//    }
//    while (is_empty_stack(st) == 1) {
//        printf("%c ->", st->cc.action);
//        st = st->next;
//    }
//}
//
///* ---------------------------------------------------------------------- */
//
//Stack pop_stack(Stack st) {
//    StackElement *element;
//    if (is_empty_stack(st) == 0)
//        return new_stack();
//
//    element = st->next;
//    return element;
//}
//
///* ---------------------------------------------------------------------- */
//
//Case top_stack(Stack st) {
//    if (is_empty_stack(st) == 0) {
//        printf("Stack vide lors du pop\n");
//        exit(EXIT_FAILURE);
//    }
//    return st->cc;
//
//}
//
///* ---------------------------------------------------------------------- */
//
//int stack_lenght(Stack st) {
//    int c = 0;
//    if (is_empty_stack(st) == 0) {
//        printf("Stack vide lors du pop\n");
//        return -1;
//    }
//
//    while (is_empty_stack(st) == 1) {
//        c++;
//        st = st->next;
//    }
//
//    return c;
//}
//
///*----------------------------------------------------------------------------------------*/
//
//Stack solveur1(terrain *t, Stack st) {
//    if (is_empty_stack(st) == 0) {
//        st = new_stack();
//    }
//
//    int run = 0;
//    while (run != 1) {
//        Case last = top_stack(st);
//        if (is_empty_stack(st) == 0) {
//            printf("Stack vide\n");
//            return st;
//        }
//
//        if (t->map[last.py - 1][last.px] == FINISH) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("UP (FINISH) ");
//        } else if (t->map[last.py][last.px + 1] == FINISH) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("RIGHT (FINISH) ");
//        } else if (t->map[last.py + 1][last.px] == FINISH) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("DOWN (FINISH) ");
//        } else if (t->map[last.py][last.px - 1] == FINISH) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("LEFT (FINISH) ");
//        } else if (t->map[last.py - 1][last.px] != WALL && t->map[last.py - 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("UP  ");
//        } else if (t->map[last.py][last.px + 1] != WALL && t->map[last.py][last.px + 1] != GHOSTWALL) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("RIGHT  ");
//        } else if (t->map[last.py + 1][last.px] != WALL && t->map[last.py + 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("DOWN  ");
//        } else if (t->map[last.py][last.px - 1] != WALL && t->map[last.py][last.px - 1] != GHOSTWALL) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("LEFT  ");
//        } else {
//            t->map[last.py][last.px] = GHOSTWALL;
//            st = pop_stack(st);
//            printf("POP  ");
//        }
//    }
//    return st;
//}
//
///*----------------------------------------------------------------------------------------*/
//
//Stack solveur2(terrain *t, Stack st) {
//
//    if (is_empty_stack(st) == 0) {
//        st = new_stack();
//    }
//    int run = 0;
//    while (run != 1) {
//        Case last = top_stack(st);
//        if (is_empty_stack(st) == 0) {
//            printf("Stack vide\n");
//            return st;
//        }
//
//        if (t->map[last.py - 1][last.px] == FINISH) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("UP (FINISH) ");
//        } else if (t->map[last.py][last.px + 1] == FINISH) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("RIGHT (FINISH) ");
//        } else if (t->map[last.py + 1][last.px] == FINISH) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("DOWN (FINISH) ");
//        } else if (t->map[last.py][last.px - 1] == FINISH) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("LEFT (FINISH) ");
//        } else if (t->map[last.py][last.px - 1] != WALL && t->map[last.py][last.px - 1] != GHOSTWALL) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("LEFT  ");
//        } else if (t->map[last.py - 1][last.px] != WALL && t->map[last.py - 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("UP  ");
//        } else if (t->map[last.py][last.px + 1] != WALL && t->map[last.py][last.px + 1] != GHOSTWALL) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("RIGHT  ");
//        } else if (t->map[last.py + 1][last.px] != WALL && t->map[last.py + 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("DOWN  ");
//
//        } else {
//            t->map[last.py][last.px] = GHOSTWALL;
//            st = pop_stack(st);
//            printf("POP  ");
//        }
//
//    }
//
//    return st;
//}
//
//
///*----------------------------------------------------------------------------------------*/
//
//Stack solveur3(terrain *t, Stack st) {
//
//    if (is_empty_stack(st) == 0) {
//        st = new_stack();
//    }
//
//    int run = 0;
//    while (run != 1) {
//        Case last = top_stack(st);
//        if (is_empty_stack(st) == 0) {
//            printf("Stack vide\n");
//            return st;
//        }
//
//        if (t->map[last.py - 1][last.px] == FINISH) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("UP (FINISH) ");
//        } else if (t->map[last.py][last.px + 1] == FINISH) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("RIGHT (FINISH) ");
//        } else if (t->map[last.py + 1][last.px] == FINISH) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("DOWN (FINISH) ");
//        } else if (t->map[last.py][last.px - 1] == FINISH) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("LEFT (FINISH) ");
//        } else if (t->map[last.py + 1][last.px] != WALL && t->map[last.py + 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("DOWN  ");
//
//        } else if (t->map[last.py][last.px - 1] != WALL && t->map[last.py][last.px - 1] != GHOSTWALL) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("LEFT  ");
//        } else if (t->map[last.py - 1][last.px] != WALL && t->map[last.py - 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("UP  ");
//        } else if (t->map[last.py][last.px + 1] != WALL && t->map[last.py][last.px + 1] != GHOSTWALL) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("RIGHT  ");
//        } else {
//            t->map[last.py][last.px] = GHOSTWALL;
//            st = pop_stack(st);
//            printf("POP  ");
//        }
//
//    }
//
//    return st;
//}
//
///*----------------------------------------------------------------------------------------*/
//
//Stack solveur4(terrain *t, Stack st) {
//
//    if (is_empty_stack(st) == 0) {
//        st = new_stack();
//    }
//
//    int run = 0;
//    while (run != 1) {
//        Case last = top_stack(st);
//        if (is_empty_stack(st) == 0) {
//            printf("Stack vide\n");
//            return st;
//        }
//
//        if (t->map[last.py - 1][last.px] == FINISH) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("UP (FINISH) ");
//        } else if (t->map[last.py][last.px + 1] == FINISH) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("RIGHT (FINISH) ");
//        } else if (t->map[last.py + 1][last.px] == FINISH) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("DOWN (FINISH) ");
//        } else if (t->map[last.py][last.px - 1] == FINISH) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            run = 1;
//            printf("LEFT (FINISH) ");
//        } else if (t->map[last.py][last.px + 1] != WALL && t->map[last.py][last.px + 1] != GHOSTWALL) {
//            Case next = {last.px + 1, last.py, 'R'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("RIGHT  ");
//        } else if (t->map[last.py + 1][last.px] != WALL && t->map[last.py + 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py + 1, 'd'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("DOWN  ");
//        } else if (t->map[last.py][last.px - 1] != WALL && t->map[last.py][last.px - 1] != GHOSTWALL) {
//            Case next = {last.px - 1, last.py, 'l'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("LEFT  ");
//        } else if (t->map[last.py - 1][last.px] != WALL && t->map[last.py - 1][last.px] != GHOSTWALL) {
//            Case next = {last.px, last.py - 1, 'U'};
//            st = push_stack(st, next, t);
//            t->map[last.py][last.px] = GHOSTWALL;
//            printf("UP  ");
//        } else {
//            t->map[last.py][last.px] = GHOSTWALL;
//            st = pop_stack(st);
//            printf("POP  ");
//        }
//
//    }
//
//    return st;
//}
//
///*----------------------------------------------------------------------------------------*/
//
//Stack solveur(terrain *t, Stack st){
//    Stack s1 = solveur1(t,st);
//    Stack s2 = solveur2(t,st);
//    Stack s3 = solveur3(t,st);
//    Stack s4 = solveur4(t,st);
//
//    int len_s1 = s1 ? stack_lenght(s1) : INT_MAX;
//    int len_s2 = s2 ? stack_lenght(s2) : INT_MAX;
//    int len_s3 = s3 ? stack_lenght(s3) : INT_MAX;
//    int len_s4 = s4 ? stack_lenght(s4) : INT_MAX;
//
//    Stack shortest = (len_s1 <= len_s2 && len_s1 <= len_s3 && len_s1 <= len_s4) ? s1 :
//                     (len_s2 <= len_s1 && len_s2 <= len_s3 && len_s2 <= len_s4) ? s2 :
//                     (len_s3 <= len_s1 && len_s3 <= len_s2 && len_s3 <= len_s4) ? s3 : s4;
//
//    return shortest;
//
//}
