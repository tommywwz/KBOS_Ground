#include "Utils.h"

void Idle(Plane *plane);

void Await_Takeoff(Plane *plane);

void Takeoff(Plane *plane);

void Flying(Plane *plane);

void Await_Landing(Plane *plane);

void Landing(Plane *plane);

Runway runways[6];
Plane cessnas[SMALL_PLANE_COUNT];
Plane airbuses[LARGE_PLANE_COUNT];
pthread_t threads[SMALL_PLANE_COUNT + LARGE_PLANE_COUNT];

int main() {
	printf("Starting KBOS Ground Control System...\n");

	FILE *fptr;
	uint32_t seed;
	fptr = fopen("seed.txt", "r");
	seed = file2int(fptr);
	srand(seed);  // set seed for random number
/******************************************************** Init ********************************************************/
	for (int i = 0; i < 6; ++i) {
		runways[i].id = i;
		runways[i].occupied_by = -1;
	}
	for (int i = 0; i < SMALL_PLANE_COUNT; ++i) {
		cessnas[i].id = i;
		cessnas[i].type = PLANE_CESSNA_172;
		cessnas[i].current_state = STATE_IDLE;
	}
	for (int i = 0; i < LARGE_PLANE_COUNT; ++i) {
		airbuses[i].id = i + SMALL_PLANE_COUNT;
		airbuses[i].type = PLANE_AIRBUS_A380;
		airbuses[i].current_state = STATE_IDLE;
	}
/******************************************************* Spawn! *******************************************************/
	for (int i = 0; i < SMALL_PLANE_COUNT; ++i) {
		// create pthread to run Idle
		pthread_create(threads + i, 0, (void *(*)(void *)) Idle, cessnas + i);
	}
	for (int i = 0; i < LARGE_PLANE_COUNT; ++i) {
		// create pthread to run Idle
		pthread_create(threads + i + SMALL_PLANE_COUNT, 0, (void *(*)(void *)) Idle, airbuses + i);
	}
/******************************************************** Wait ********************************************************/
	for (int i = 0; i < SMALL_PLANE_COUNT; ++i) {
		pthread_join(threads[i], 0);
	}
	return 0;
}

void Idle(Plane *plane) {
	plane->current_state = STATE_IDLE;
	// get random time, announce intended sleep duration
	int sleep_time = rand() % 10000000; // 0 - 10 seconds

	char str[100];
	print_helper(str, plane);
	printf("%s: sleep for %7d μs before takeoff\n", str, sleep_time);

	usleep(sleep_time);
	Await_Takeoff(plane);
}

void Await_Takeoff(Plane *plane) {
	printf("Await_Takeoff\n");
	plane->current_state = STATE_AWAIT_TAKEOFF;
	// todo chose random set of runways for takeoff
	// todo chose random order to use the set of runways
	// todo print intended runway order
	// todo check if runways are occupied
	// todo if occupied, print a note to the console indicating it must wait
	// todo sleep on a semaphore until it is able to proceed
	// todo if not occupied, occupy the runways and proceed to takeoff, print to console that it is doing so
	Takeoff(plane);
}

void Takeoff(Plane *plane) {
	plane->current_state = STATE_TAKEOFF;
	// todo Loop: repeat until the plane left the last region
	// todo proceed to the next region without delay and announce its ident and position
	// todo check if a plane is in the same runway && same region, if so BAD DAY, else ALIVE
	// todo usleep for a random time
	Flying(plane);
}

void Flying(Plane *plane) {
	plane->current_state = STATE_FLYING;
	// todo get random time, announce intended sleep duration
	// todo usleep
	Await_Landing(plane);
}

void Await_Landing(Plane *plane) {
	plane->current_state = STATE_AWAIT_LANDING;
	// todo chose random set of runways for takeoff
	// todo chose random order to use the set of runways
	// todo print intended runway order
	// todo check if runways are occupied
	// todo if occupied, print a note to the console indicating it must wait
	// todo sleep on a semaphore until it is able to proceed
	// todo if not occupied, occupy the runways and proceed to takeoff, print to console that it is doing so
	Landing(plane);
}

void Landing(Plane *plane) {
	plane->current_state = STATE_LANDING;
	// todo Loop: repeat until the plane left the last region
	// todo proceed to the next region without delay and announce its ident and position
	// todo check if a plane is in the same runway && same region, if so BAD DAY, else ALIVE
	// todo usleep for a random time
//	Idle(plane);
}
