#include "Utils.h"

void Idle(Plane *plane);

void Await_Takeoff(Plane *plane);

void Takeoff(Plane *plane);

void Flying(Plane *plane);

void Await_Landing(Plane *plane);

void Landing(Plane *plane);

//Short_Runway Short_Runways[6];
//Long_Runway Long_Runways[2];
Runway runways[4];
Plane cessnas[SMALL_PLANE_COUNT];
Plane airbuses[LARGE_PLANE_COUNT];
pthread_t threads[SMALL_PLANE_COUNT + LARGE_PLANE_COUNT];

runways_combinations long_runway_combs_pool[] = {RUNWAY_1_4_6, RUNWAY_2_3_5};
runways_combinations short_runway_combs_pool[] = {RUNWAY_1_4, RUNWAY_4_6,
                                                  RUNWAY_2_3, RUNWAY_3_5,
                                                  RUNWAY_1_2, RUNWAY_3_4};

int runway_combs_pool[][3] = {{1, 4, 6}, {1, 4}, {4, 6},
                              {2, 3, 5}, {2, 3}, {3, 5},
                              {1, 2}, {3, 4}};

int main() {
	printf("Starting KBOS Ground Control System...\n");

	FILE *fptr;
	uint32_t seed;
	fptr = fopen("seed.txt", "r");
	seed = file2int(fptr);
	srand(seed);  // set seed for random number
/******************************************************** Init ********************************************************/
for (int i = 0; i < 4; i++) {
    runways[i].name = (runway_identifiers)i;
    runways[i].queue_counter = 0;
}
//	for (int i = 0; i < LONG_RWY_COUNT; ++i) {
//        // initialize long runways
//        int *src_ptr = long_rwys[i];
//        Long_Runways[i].name = src_ptr;
//        Long_Runways[i].if_cleared = 1;
//	}
//    for (int i = 0; i < SHORT_RWY_COUNT; ++i) {
//        // initialize short runways
//        int *src_ptr = short_rwys[i];
//        Short_Runways[i].name = src_ptr;
//        Short_Runways[i].if_cleared = 1;
//    }
//    debug:
//    printf("2nd long runway %d, %d, %d\n",
//           Long_Runways[1].name[0],
//           Long_Runways[1].name[1],
//           Long_Runways[1].name[2]);
//    return 0;
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
	printf("%s: sleep for %7d μs at terminal\n", str, sleep_time);

	usleep(sleep_time);
	Await_Takeoff(plane);
}

void Await_Takeoff(Plane *plane) {
	printf("Await_Takeoff\n");
	plane->current_state = STATE_AWAIT_TAKEOFF;

	// todo chose random set of runways for takeoff
    plane_type ptype = plane->type;
    int* ptr_my_runway = plane->myRunway;
    int runway_size;

    runways_combinations my_runway_comb_key; // the key to my runway combination
    // obtaining a runway combination by the size of the plane
    if (ptype == PLANE_AIRBUS_A380) {
        runway_size = 3;
        int runway_key = rand() % LONG_RWY_COUNT;
        my_runway_comb_key = long_runway_combs_pool[runway_key];
    } else {
        runway_size = 2;
        int runway_key = rand() % SHORT_RWY_COUNT;
        my_runway_comb_key = short_runway_combs_pool[runway_key];
    }
    // find the runway identifier of my runway combination
    runway_identifiers runway_ident = find_runway_ident(my_runway_comb_key);

    // todo chose random order to use the set of runways
    int* ptr_my_combination = runway_combs_pool[my_runway_comb_key]; // get my runway combination using the key

    int reverse_takeoff = rand() % 2;  // 0 is default direction, 1 is reverse direction

    copyArray(ptr_my_combination, ptr_my_runway, runway_size, reverse_takeoff);

    //todo shared data protection
    runways[runway_ident].queue_counter ++;
    //todo ^ sensitive region ^

	// todo print intended runway order
    char str[100];
    print_helper(str, plane);
    printf("%s: Lineup at Runway ", str);
    for(int i=0; i<runway_size; i++) {
        printf("%d", ptr_my_runway[i]);
    }
    printf("\n");

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
