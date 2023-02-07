#include "Utils.h"

char *state_to_string(state s) {
	switch (s) {
		case STATE_IDLE:
			return "Idle";
		case STATE_AWAIT_TAKEOFF:
			return "Await Takeoff";
		case STATE_TAKEOFF:
			return "Takeoff";
		case STATE_FLYING:
			return "Flying";
		case STATE_AWAIT_LANDING:
			return "Await Landing";
		case STATE_LANDING:
			return "Landing";
		default:
			return "Unknown";
	}
}

void print_helper(char *str, Plane *plane) {
	sprintf(str,
	        "[Plane Thread %2d], type [%s], state [%s]",
	        plane->id,
	        plane->type == PLANE_CESSNA_172 ?
	        "small" : "LARGE",
	        state_to_string(plane->current_state));
}

void get_real_runway(Plane *plane) {
	// plane->myRunway to string
	char *str = malloc(4);

	if (plane->type == PLANE_CESSNA_172)
		sprintf(str, "%d%d", plane->myRunway[0], plane->myRunway[1]);
	else if (plane->type == PLANE_AIRBUS_A380)
		sprintf(str, "%d%d%d", plane->myRunway[0], plane->myRunway[1], plane->myRunway[2]);

	if (strcmp(str, "146") == 0 ||
	    strcmp(str, "641") == 0 ||
	    strcmp(str, "235") == 0 ||
	    strcmp(str, "532") == 0) {
		for (int i = 0; i < 3; ++i) {
			if (plane->myRunway[i] < 5)
				plane->realRunway[i] = plane->myRunway[i] + 6;
			else
				plane->realRunway[i] = plane->myRunway[i];

		}
	} else if (strcmp(str, "14") == 0 ||
	           strcmp(str, "41") == 0 ||
	           strcmp(str, "23") == 0 ||
	           strcmp(str, "32") == 0 ||
	           strcmp(str, "35") == 0 ||
	           strcmp(str, "53") == 0 ||
	           strcmp(str, "46") == 0 ||
	           strcmp(str, "64") == 0) {
		for (int i = 0; i < 2; ++i) {
			if (plane->myRunway[i] < 5)
				plane->realRunway[i] = plane->myRunway[i] + 6;
			else
				plane->realRunway[i] = plane->myRunway[i];
		}
	} else {
		for (int i = 0; i < 2; ++i) {
			plane->realRunway[i] = plane->myRunway[i];
		}
	}
}

int file2int(FILE *ptr) {
	// reference: https://stackoverflow.com/questions/3747086/reading-the-whole-text-file-into-a-char-array-in-c
	long lSize;
	char *buffer; // buffer that stores the converted strings from file
	int number;

	if (NULL == ptr) {
		printf("Error opening file: seed.txt");
		exit(1);
	}

	fseek(ptr, 0L, SEEK_END);
	lSize = ftell(ptr); // get the length of the text file
	rewind(ptr); // rewind the ptr to the beginning of the file

	buffer = calloc(1, lSize + 1);

	if (!buffer) { // check if memory are allocated successfully
		fclose(ptr);
		printf("failed to allocate memory");
		exit(1);
	}

	if (1 != fread(buffer, lSize, 1, ptr)) {
		fclose(ptr);
		printf("reading fails");
		exit(1);
	}
	// Closing the file
	fclose(ptr);

	printf("Read seed value: %s\n\n", buffer);
	number = atoi(buffer); // convert buffer string to integer
	printf("Read seed value (converted to integer): %d\n", number);

	free(buffer);  // free buffer
	return number;
}
