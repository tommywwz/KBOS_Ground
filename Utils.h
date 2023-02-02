#ifndef KBOS_GROUND_UTILS_H
#define KBOS_GROUND_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define SHORT_RWY_COUNT 6
#define LONG_RWY_COUNT 2
#define SMALL_PLANE_COUNT 12
#define LARGE_PLANE_COUNT 6
#define RUNWAY_COMBINATIONS_COUNT 8

typedef enum state {
	STATE_IDLE,
	STATE_AWAIT_TAKEOFF,
	STATE_TAKEOFF,
	STATE_FLYING,
	STATE_AWAIT_LANDING,
	STATE_LANDING
} state;

typedef enum plane_type {
	PLANE_CESSNA_172,
	PLANE_AIRBUS_A380
} plane_type;

//typedef enum runway_combination_long {
//	RUNWAY_1_4_6,
//	RUNWAY_2_3_5
//} runway_combination_long;
//
//typedef enum runway_combination_short {
//    RUNWAY_1_2,
//    RUNWAY_3_4,
//    RUNWAY_1_4,
//    RUNWAY_2_3,
//    RUNWAY_3_5,
//    RUNWAY_4_6
//} runway_combination_short;

typedef enum runway_identifiers {
    RUNWAY146 = 0,
    RUNWAY235 = 1,
    RUNWAY12 = 2,
    RUNWAY34 = 3
} runway_identifiers;

typedef enum runways_combinations {
    RUNWAY_1_4_6 = 0,
    RUNWAY_1_4 = 1,
    RUNWAY_4_6 = 2,
    RUNWAY_2_3_5 = 3,
    RUNWAY_2_3 = 4,
    RUNWAY_3_5 = 5,
    RUNWAY_1_2 = 6,
    RUNWAY_3_4 = 7,
} runways_combinations;

typedef struct Runway {
    runway_identifiers name;
    int queue_counter;
} Runway;

typedef struct Long_Runway {
    int *name;
    int if_cleared;
    int lineup_Queue[LARGE_PLANE_COUNT];
} Long_Runway;

typedef struct Short_Runway {
    int *name;
    int if_cleared;
	int lineup_Queue[SMALL_PLANE_COUNT];
} Short_Runway;

typedef struct Plane {
	int id;
    int myRunway[3];
	plane_type type;
	state current_state;
} Plane;

char *state_to_string(state s);

runway_identifiers find_runway_ident (int rwy_key);

void copyArray(int* src_arr, int* dest_arr, int size, int reverse);

int file2int(FILE *ptr);

void print_helper(char *str, Plane *plane);

#endif //KBOS_GROUND_UTILS_H
