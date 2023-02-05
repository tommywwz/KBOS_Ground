#ifndef KBOS_GROUND_UTILS_H
#define KBOS_GROUND_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define YEL   "\x1B[33m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define GRN   "\x1B[32m"
#define BLU  "\x1B[34m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define RWY_REGIONS_COUNT 6
#define SHORT_RWY_COUNT 6
#define LONG_RWY_COUNT 2
#define SMALL_PLANE_COUNT 30
#define LARGE_PLANE_COUNT 15

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

typedef struct runway_combinations_long_sems {
	// array of runway semaphores
	sem_t* sem_runway_regions[3];
	int runway_name[3];
} runway_combinations_long_sems;

typedef struct runway_combinations_short_sems {
	// array of runway semaphores
	sem_t* sem_runway_regions[2];
	int runway_name[2];
} runway_combinations_short_sems;

typedef struct Plane {
	int id;
    int myRunway[3];
	plane_type type;
	state current_state;
} Plane;

char *state_to_string(state s);

void copyArray(int* src_arr, int* dest_arr, int size, int reverse);

int file2int(FILE *ptr);

void print_helper(char *str, Plane *plane);

#endif //KBOS_GROUND_UTILS_H
