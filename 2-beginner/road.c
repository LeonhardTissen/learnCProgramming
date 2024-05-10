#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char getChar(int i, int time) {
	int charIndex = (i + time) % 26;
	return alphabet[charIndex];
}

void printSegment(int start, int end, int time) {
	int total = 90;

	char* segment = malloc(total + 1);

	for (int i = 0; i < start; i++) {
		segment[i] = getChar(i, time);
	}
	for (int i = start; i < end; i++) {
		segment[i] = ' ';
	}
	for (int i = end; i < total; i++) {
		segment[i] = getChar(i, time);
	}

	printf("%s\n", segment);
}

int main() {
	int time = 0;
	int width = 32;
	int speed = 15;
	int maxSpeed = 130;
	bool isSlowingDown = false;

	for (;;) {
		int x = sin(time / 10.0) * 20 + 28;

		// Increase the time
		time++;

		// Print the segment
		printSegment(x, x + width, time);

		// Change the speed
		if (isSlowingDown) {
			if (time % 1 == 0) {
				speed--;
			}
		} else {
			if (time % 2 == 0) {
				speed++;
			}
		}

		// Change state if going too fast or too slow
		if (speed == 15) {
			isSlowingDown = false;
		} else if (speed == maxSpeed && !isSlowingDown) {
			if (width > 0) {
				width--;
			}
			isSlowingDown = true;
		}

		// Wait before the next frame
		msleep(1000 / speed);
	}
}

