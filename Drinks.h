#ifndef Boards_h
#define Boards_h

#define TEMP 1

typedef struct
{
  char name[];
  int numValves;
  int valves[];
  int timings[];
} Drink;


Drink orange_juice = { .name[12] = "Orange Juice",
                        .numValves = 3,
                        .valves[3] = {30, 35, 40},
                        .timings[3] = {3000, 5000, 2000}
                      };
                        

Drink drinks[1] = {orange_juice};

#endif
