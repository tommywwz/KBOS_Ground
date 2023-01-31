#ifndef KBOS_GROUND_UTILS_H
#define KBOS_GROUND_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#define SMALL_PLANE_COUNT 12
#define LARGE_PLANE_COUNT 6

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

typedef struct Runway {
	int id;
	int occupied_by;
} Runway;

typedef struct Plane {
	int id;
	plane_type type;
	state current_state;
} Plane;

typedef enum runway_combination_long {
	RUNWAY_1_4_6,
	RUNWAY_2_3_5
} runway_combination_long;

typedef enum runway_combination_short {
	RUNWAY_1_2,
	RUNWAY_3_4,
	RUNWAY_1_4,
	RUNWAY_2_3,
	RUNWAY_3_5,
	RUNWAY_4_6
} runway_combination_short;

char *state_to_string(state s);

int file2int(FILE *ptr);

void print_helper(char *str, Plane *plane);

#endif //KBOS_GROUND_UTILS_H
