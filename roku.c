#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "roku.h"

static const char chars[4][8] = { {'A', 'B', 'C', 'D', 'E', 'F', 'G', 8},
                     {'H', 'I', 'J', 'K', 'L', 'M', 'N', 27},
                     {'O', 'P', 'Q', 'R', 'S', 'T', 'U', 0},
                     {'V', 'W', 'X', 'Y', 'Z', '.', '\'', 0} };

struct Directions getDirections(struct IndicesOfChar start, struct IndicesOfChar end) {
	//printf("Start Row: %d\tStart Col: %d\nEnd Row: %d\tEnd Col: %d\n", start.row, start.col, end.row, end.col);
	struct Directions directionsToReturn;
	directionsToReturn.horizontal = end.col - start.col;
	directionsToReturn.vertical = end.row - start.row;
	return directionsToReturn;
}

void navigateToCharacter(struct Directions dir, CURL *myCurl, int shouldSendKey) {
	int delay = 0.35 * 1E6; //microseconds

	usleep(delay);

	if (dir.horizontal > 0) {
		while (dir.horizontal > 0) {
			sendPost(myCurl, "keypress", "right");
			dir.horizontal--;
			usleep(delay);
		}
	}
	else if (dir.horizontal < 0) {
		while (dir.horizontal < 0) {
			sendPost(myCurl, "keypress", "left");
			dir.horizontal++;
			usleep(delay);
		}
	}

	if (dir.vertical > 0) {
		while (dir.vertical > 0) {
			sendPost(myCurl, "keypress", "down");
			dir.vertical--;
			usleep(delay);
		}
	}
	else if (dir.vertical < 0) {
		while (dir.vertical < 0) {
			sendPost(myCurl, "keypress", "up");
			dir.vertical++;
			usleep(delay);
		}
	}

	usleep(1.5 * delay);

	if (shouldSendKey == 1) {
		sendPost(myCurl, "keypress", "select");
	}
}

struct IndicesOfChar getIndices(char input) {
	int i = 0, j = 0;
	int found = 0;
	struct IndicesOfChar indicesToReturn;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			if (chars[i][j] == input) {
				indicesToReturn.row = i;
				indicesToReturn.col = j;
				found = 1;
			}
		}
	}
	return indicesToReturn;
}

void getRowAndColumn(char c) {
	int i = 0, j = 0;
	int found = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			if (chars[i][j] == c) {
				//printf("Row: %d\tColumn: %d\n", i, j);
				found = 1;
			}
		}
	}
	if (found == 0) {
		printf("Character %c not found.\n", c);
	}
}

void printIndices(struct IndicesOfChar myIndices) {
	printf("Row: %d\nColumn: %d\nCharacter in the matrix: %c\n", myIndices.row, myIndices.col, chars[myIndices.row][myIndices.col]);
}

void sendPost(CURL *myCurl, char *myCmd, char *myTarget) {
	char *rokuIP = "192.168.2.4:8060";  //change to IP of roku device, port 8060
	char myURL[50] = "http://";
	strcat(myURL, rokuIP);
	strcat(myURL, "/");
	strcat(myURL, myCmd);
	strcat(myURL, "/");
	strcat(myURL, myTarget);
	//printf("resulting URL: %s\n", myURL);

	CURLcode res;
	static const char *postthis = "";

	if (myCurl) {
		curl_easy_setopt(myCurl, CURLOPT_URL, myURL);
		curl_easy_setopt(myCurl, CURLOPT_POSTFIELDS, postthis);

		curl_easy_setopt(myCurl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));

		res = curl_easy_perform(myCurl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
	}
}

void goToYoutubeKeyboard(CURL *myCurl) {
	//add delays if needed
	sendPost(myCurl, "keypress", "left");
	sendPost(myCurl, "keypress", "left");
	sendPost(myCurl, "keypress", "up");
	sendPost(myCurl, "keypress", "right");
	sendPost(myCurl, "keypress", "right");
}

void homeOrYoutube(CURL *myCurl) {
	while (1) {
		printf("Press 1 to go home or 2 to go to Youtube.\n");
		char c = getchar();
		if (c == '1') {
			sendPost(myCurl, "keypress", "Home");
			while (1) {
				getchar();
				printf("Press enter when at the home screen to go to Youtube.\n");
				c = getchar();

				sendPost(myCurl, "launch", "837");
				break;
			}
			break;
		}
		else if (c == '2') {
			sendPost(myCurl, "launch", "837");
			getchar(); //remove trailing newline
			break;
		}
	}

	while (1) {
		printf("Press enter to navigate to the keyboard.\n");
		char c = getchar();
		if (c == '\n') {
			goToYoutubeKeyboard(myCurl);
			break;
		}
	}

}

void playingVideo(CURL *myCurl, int *myUserComesFromVideo) {
	printf("Now playing video\nEnter 1 to return to keyboard or 2 to browse suggestions.\n");
	char c = getchar();
	getchar(); //remove trailing newline
	if (c == '1') {
		printf("Returning to keyboard.\n");
		*myUserComesFromVideo = 0;
	}
	else if (c == '2') {
		printf("Returning to first suggestion.\n");
		*myUserComesFromVideo = 1;
	}
	sendPost(myCurl, "keypress", "back");
}

void interpretKeyboard(CURL *myCurl, int *myUserComesFromVideo) {
	int numRight = 0;
	while (1) {
		printf("Enter \'a\' to move left, \'d\' to move right, or \'s\' to select the currently hightlighted video.\n");
		char c = getchar();
		getchar(); //remove trailing newline
		if (c == 'a' && numRight > 0) {
			printf("Moving left.\n");
			sendPost(myCurl, "keypress", "left");
			numRight--;
		}
		else if (c == 'd') {
			printf("Moving right.\n");
			sendPost(myCurl, "keypress", "right");
			numRight++;
		}
		else if (c == 's') {
			printf("Selecting current video; sending control to playVido function.\n");
			sendPost(myCurl, "keypress", "select");
			playingVideo(myCurl, myUserComesFromVideo);
			break;
		}
	}
}

void takeInput(CURL *myCurl, int *myUserPreviouslyEnteredText) {
	char currentChar = chars[0][0];

	int startFlag = 1;
	struct IndicesOfChar startIndices = getIndices(currentChar);

	if (*myUserPreviouslyEnteredText == 1) {
		//user is returning from video and desires a new query
		//cursor is now at first suggestion - up, up, right, right, right, right, right, right, right, down, select,
		//	up, left, left, left, left, left, left, left - will place cursor back at 'A'
		usleep(0.35 * 1E6);
		sendPost(myCurl, "keypress", "up");
		usleep(0.35 * 1E6);
		sendPost(myCurl, "keypress", "up");
		usleep(0.35 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "right");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "down");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "select");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "up");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
		sendPost(myCurl, "keypress", "left");
		usleep(0.45 * 1E6);
	}

	while (1) {
		if (startFlag) {
			printf("\nPlease enter a character, or ctrl + d to quit.\n");
		}
		char c = getchar();
		if (c == EOF) break;
		else if (c != '\n') {
			if (isalpha(c)) {
				c = toupper(c);
				startIndices = getIndices(currentChar);
				//printIndices(startIndices); //uncomment for debugging
				printf("You entered: %c\n", c);
				struct IndicesOfChar endIndices = getIndices(c);
				//printIndices(endIndices); //uncomment for debugging
				struct Directions myDirections = getDirections(startIndices, endIndices);
				navigateToCharacter(myDirections, myCurl, 1);
				currentChar = c;
				startFlag = 1;
			}
			else if (c == ' ') {
				printf("You entered: SPACE\n");
				c = 'V';
				startIndices = getIndices(currentChar);
				struct IndicesOfChar endIndices = getIndices(c);
				struct Directions myDirections = getDirections(startIndices, endIndices);
				navigateToCharacter(myDirections, myCurl, 0);
				sendPost(myCurl, "keypress", "down");
				usleep(0.35 * 1E6);
				sendPost(myCurl, "keypress", "select");
				usleep(0.35 * 1E6);
				sendPost(myCurl, "keypress", "up");
				usleep(0.35 * 1E6);
				currentChar = c;
			}
			else {
				printf("Non-alphabetic characters are currently unsupported.\n");
			}
		}
		else startFlag = 0;
	}
	*myUserPreviouslyEnteredText = 1;
}

void navigateToFirstSuggestion(CURL *myCurl, int *myUserComesFromVideo) {
	if (*myUserComesFromVideo == 0) {
		//scroll down from last selected character on keyboard
		//worst case - first suggestion is five rows down
		int delay = 0.25 * 1E6;
		//usleep(delay); //uncomment if needed
		sendPost(myCurl, "keypress", "down");
		usleep(delay);
		sendPost(myCurl, "keypress", "down");
		usleep(delay);
		sendPost(myCurl, "keypress", "down");
		usleep(delay);
		sendPost(myCurl, "keypress", "down");
		usleep(delay);
		sendPost(myCurl, "keypress", "down");
		usleep(delay);
	}
	interpretKeyboard(myCurl, myUserComesFromVideo);
}

int main(int argc, char* argv[]) {
	CURL *curl;
	curl = curl_easy_init();

	homeOrYoutube(curl);

	int userPreviouslyEnteredText = 0;
	int userComesFromVideo = 0; // 1 means that the user is returning from a video - place cursor on first suggestion

	while (1) {
		if (!userComesFromVideo) {
			takeInput(curl, &userPreviouslyEnteredText);
		}
		navigateToFirstSuggestion(curl, &userComesFromVideo);
	}

	curl_easy_cleanup(curl);

	return 0;
}
