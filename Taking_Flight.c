#include "Utils.h"

void Idle(Plane *plane);

void Await_Takeoff(Plane *plane);

void Takeoff(Plane *plane);

void Flying(Plane *plane);

void Await_Landing(Plane *plane);

void Landing(Plane *plane);

Plane cessnas[SMALL_PLANE_COUNT];
Plane airbuses[LARGE_PLANE_COUNT];
pthread_t threads[SMALL_PLANE_COUNT + LARGE_PLANE_COUNT];

sem_t semaphores[RWY_REGIONS_COUNT + 1]; // 1-based indexing
runway_combinations_long_sems long_runways[LONG_RWY_COUNT];
runway_combinations_short_sems short_runways[SHORT_RWY_COUNT];

int runway_combs_pool_short[SHORT_RWY_COUNT][2] = {{1, 4},
                                                   {4, 6},
                                                   {2, 3},
                                                   {3, 5},
                                                   {1, 2},
                                                   {3, 4}};

int runway_combs_pool_long[LONG_RWY_COUNT][3] = {{1, 4, 6},
                                                 {2, 3, 5}};

int main() {
	printf("Starting KBOS Ground Control System...\n");

	FILE *fptr;
	uint32_t seed;
	fptr = fopen("seed.txt", "r");
	seed = file2int(fptr);
	srand(seed);  // set seed for random number

/******************************************************** Init ********************************************************/

	// init semaphores
	for (int i = 0; i < RWY_REGIONS_COUNT; ++i) {
		sem_init(semaphores + 1 + i, 0, 1);
	}

	// init runway_combinations_long_sems
	for (int i = 0; i < LONG_RWY_COUNT; ++i) {
		for (int j = 0; j < 3; ++j) {
			long_runways[i].sem_runway_regions[j] = &semaphores[runway_combs_pool_long[i][j]];
			long_runways[i].runway_name[j] = runway_combs_pool_long[i][j];
		}
	}
	for (int i = 0; i < SHORT_RWY_COUNT; ++i) {
		for (int j = 0; j < 2; ++j) {
			short_runways[i].sem_runway_regions[j] = &semaphores[runway_combs_pool_short[i][j]];
			short_runways[i].runway_name[j] = runway_combs_pool_short[i][j];
		}
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
	for (int i = 0; i < SMALL_PLANE_COUNT + LARGE_PLANE_COUNT; ++i) {
		pthread_join(threads[i], 0);
	}
	printf("KBOS Ground Control System shutting down...\n");
	return 0;
}

void Idle(Plane *plane) {
	plane->current_state = STATE_IDLE;
	// get random time, announce intended sleep duration
	int sleep_time = rand() % 10000000; // 0 - 10 seconds

	char str[100];
	print_helper(str, plane);
	printf(" ------ %s: sleep for %7d μs at terminal\n", str, sleep_time);

	usleep(sleep_time);
	Await_Takeoff(plane);
}

void Await_Takeoff(Plane *plane) {
	plane->current_state = STATE_AWAIT_TAKEOFF;
	char str[100];
	print_helper(str, plane);
	printf(" ------ %s: Awaiting Takeoff\n", str);

	// chose random order to use the set of runways
	int reverse = rand() % 2;
	int my_runway_size;
	int random_index;

	// chose random set of runways for takeoff
	if (plane->type == PLANE_AIRBUS_A380) {
		my_runway_size = 3;
		random_index = rand() % LONG_RWY_COUNT;
		if (reverse) {
			plane->myRunway[0] = long_runways[random_index].runway_name[2];
			plane->myRunway[1] = long_runways[random_index].runway_name[1];
			plane->myRunway[2] = long_runways[random_index].runway_name[0];
		} else {
			plane->myRunway[0] = long_runways[random_index].runway_name[0];
			plane->myRunway[1] = long_runways[random_index].runway_name[1];
			plane->myRunway[2] = long_runways[random_index].runway_name[2];
		}
	} else {
		my_runway_size = 2;
		random_index = rand() % SHORT_RWY_COUNT;
		if (reverse) {
			plane->myRunway[0] = short_runways[random_index].runway_name[1];
			plane->myRunway[1] = short_runways[random_index].runway_name[0];
		} else {
			plane->myRunway[0] = short_runways[random_index].runway_name[0];
			plane->myRunway[1] = short_runways[random_index].runway_name[1];
		}
	}

	// print intended runway order
	printf("%s: Lineup at Runway ", str);
	for (int i = 0; i < my_runway_size; ++i) {
		printf("%d", plane->myRunway[i]);
	}
	printf("\n");

	// wait for runways and occupy them
	if (plane->type == PLANE_AIRBUS_A380) {
		for (int i = 0; i < 3; ++i) { // for each runway
			if (sem_trywait(long_runways[random_index].sem_runway_regions[i]) == -1) { // try to occupy the runway
				// if occupied, print a note to the console indicating it must wait
				printf("%s: Waiting - runway %d is occupied\n", str, plane->myRunway[i]);
				// sleep on a semaphore until it is able to proceed
				sem_wait(long_runways[random_index].sem_runway_regions[i]);
			} else {
				// if not occupied, occupy the runways and proceed to take off, print to console that it is doing so
				printf("%s: Occupying runway %d\n", str, plane->myRunway[i]);
			}
//			printf("%s: DEBUG: Checking Availability runway %d\n", str, plane->myRunway[i]);
		}
	} else {
		for (int i = 0; i < 2; ++i) { // for each runway
			if (sem_trywait(short_runways[random_index].sem_runway_regions[i]) == -1) {
				// if occupied, print a note to the console indicating it must wait
				printf("%s: Waiting - runway %d is occupied\n", str, plane->myRunway[i]);

				// sleep on a semaphore until it is able to proceed
				sem_wait(short_runways[random_index].sem_runway_regions[i]);
			} else {
				// if not occupied, occupy the runways and proceed to take off, print to console that it is doing so
				printf("%s: Occupying runway %d\n", str, plane->myRunway[i]);
			}
//			printf("%s: DEBUG: Checking Availability runway %d\n", str, plane->myRunway[i]);
		}
	}
	Takeoff(plane);
}

void Takeoff(Plane *plane) {
	plane->current_state = STATE_TAKEOFF;
	// todo (or not todo?) check if a plane is in the same runway && same region, if so BAD DAY, else ALIVE

	char str[100];
	print_helper(str, plane);

	printf(" ------ %s: Clear to departure\n", str);
	// release runways
	if (plane->type == PLANE_AIRBUS_A380) {
		for (int i = 0; i < 3; ++i) {   // Loop: repeat until the plane left the last region
			// get random time, announce intended sleep duration
			int sleep_time = rand() % 1000000; // 0 - 1 seconds

			print_helper(str, plane);  // load plane information
			printf("%s: is rolling to runway %d\n", str, plane->myRunway[i]);
			usleep(sleep_time); // usleep for a random time
			sem_post(&semaphores[plane->myRunway[i]]); // post current runway region and roll to the next runway region
		}
	} else {
		for (int i = 0; i < 2; ++i) {   // Loop: repeat until the plane left the last region
			// get random time, announce intended sleep duration
			int sleep_time = rand() % 1000000; // 0 - 1 seconds

			print_helper(str, plane);   // load plane information
			printf("%s: is rolling to runway %d\n", str, plane->myRunway[i]);
			usleep(sleep_time); // usleep for a random time
			sem_post(&semaphores[plane->myRunway[i]]); // post current runway region and roll to the next runway region
		}
	}
	Flying(plane);
}

void Flying(Plane *plane) {
	plane->current_state = STATE_FLYING;
    char str[100];
    print_helper(str, plane);
    printf(" ------ %s: Climb and Maintain %d000, Performing Airfield Traffic Pattern\n", str, rand() % 4 + 3);
	// get random time, announce intended sleep duration
	int sleep_time = rand() % 10000000; // 0 - 10 seconds

	printf("%s: sleep for %7d μs in the air\n", str, sleep_time);
	usleep(sleep_time);
	Await_Landing(plane);
}

void Await_Landing(Plane *plane) {
	plane->current_state = STATE_AWAIT_LANDING;
	char str[100];
	print_helper(str, plane);
	printf(" ------ %s: Inbound for Landing\n", str);

	// chose random order to use the set of runways
	int reverse = rand() % 2;
	int my_runway_size;
	int random_index;

	// chose random set of runways for landing
	if (plane->type == PLANE_AIRBUS_A380) {
		my_runway_size = 3;
		random_index = rand() % LONG_RWY_COUNT;
		if (reverse) {
			plane->myRunway[0] = long_runways[random_index].runway_name[2];
			plane->myRunway[1] = long_runways[random_index].runway_name[1];
			plane->myRunway[2] = long_runways[random_index].runway_name[0];
		} else {
			plane->myRunway[0] = long_runways[random_index].runway_name[0];
			plane->myRunway[1] = long_runways[random_index].runway_name[1];
			plane->myRunway[2] = long_runways[random_index].runway_name[2];
		}
	} else {
		my_runway_size = 2;
		random_index = rand() % SHORT_RWY_COUNT;
		if (reverse) {
			plane->myRunway[0] = short_runways[random_index].runway_name[1];
			plane->myRunway[1] = short_runways[random_index].runway_name[0];
		} else {
			plane->myRunway[0] = short_runways[random_index].runway_name[0];
			plane->myRunway[1] = short_runways[random_index].runway_name[1];
		}
	}

	// print intended runway order
	printf("%s: Request Landing at Runway ", str);
	for (int i = 0; i < my_runway_size; ++i) {
		printf("%d", plane->myRunway[i]);
	}
	printf("\n");

	// wait for runways and occupy them
	if (plane->type == PLANE_AIRBUS_A380) {
		for (int i = 0; i < 3; ++i) { // for each runway
			if (sem_trywait(long_runways[random_index].sem_runway_regions[i]) == -1) { // try to occupy the runway
				// if occupied, print a note to the console indicating it must wait
				printf("%s: Waiting - runway %d is occupied\n", str, plane->myRunway[i]);
				// sleep on a semaphore until it is able to proceed
				sem_wait(long_runways[random_index].sem_runway_regions[i]);
			} else {
				// if not occupied, occupy the runways and proceed to take off, print to console that it is doing so
				printf("%s: Occupying runway %d\n", str, plane->myRunway[i]);
			}
//			printf("%s: DEBUG: Checking Availability runway %d\n", str, plane->myRunway[i]);
		}
	} else {
		for (int i = 0; i < 2; ++i) { // for each runway
			if (sem_trywait(short_runways[random_index].sem_runway_regions[i]) == -1) {
				// if occupied, print a note to the console indicating it must wait
				printf("%s: Waiting - runway %d is occupied\n", str, plane->myRunway[i]);

				// sleep on a semaphore until it is able to proceed
				sem_wait(short_runways[random_index].sem_runway_regions[i]);
			} else {
				// if not occupied, occupy the runways and proceed to take off, print to console that it is doing so
				printf("%s: Occupying runway %d\n", str, plane->myRunway[i]);
			}
//			printf("%s: DEBUG: Checking Availability runway %d\n", str, plane->myRunway[i]);
		}
	}

	Landing(plane);
}

void Landing(Plane *plane) {
	plane->current_state = STATE_LANDING;
	// todo (or not todo?) check if a plane is in the same runway && same region, if so BAD DAY, else ALIVE

	char str[100];
	print_helper(str, plane);

	printf(" ------ %s: Cleared to land\n", str);
	// release runways
	if (plane->type == PLANE_AIRBUS_A380) {
		for (int i = 0; i < 3; ++i) {   // Loop: repeat until the plane left the last region
			// get random time, announce intended sleep duration
			int sleep_time = rand() % 1000000; // 0 - 1 seconds

			print_helper(str, plane);  // load plane information
			printf("%s: is landing on runway %d\n", str, plane->myRunway[i]);
			usleep(sleep_time); // usleep for a random time
			sem_post(&semaphores[plane->myRunway[i]]); // post current runway region and roll to the next runway region
		}
	} else {
		for (int i = 0; i < 2; ++i) {   // Loop: repeat until the plane left the last region
			// get random time, announce intended sleep duration
			int sleep_time = rand() % 1000000; // 0 - 1 seconds

			print_helper(str, plane);   // load plane information
			printf("%s: is landing on runway %d\n", str, plane->myRunway[i]);
			usleep(sleep_time); // usleep for a random time
			sem_post(&semaphores[plane->myRunway[i]]); // post current runway region and roll to the next runway region
		}
	}
	printf("%s: Taxing to terminal, Goodbye KBOS Ground!\n", str);
	Idle(plane);
}
