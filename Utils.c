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
