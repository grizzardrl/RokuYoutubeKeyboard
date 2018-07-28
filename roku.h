#ifndef ROKU_HEADER
#define ROKU_HEADER

#include <curl/curl.h>

char chars[4][8] = { {'A', 'B', 'C', 'D', 'E', 'F', 'G', 8},
                     {'H', 'I', 'J', 'K', 'L', 'M', 'N', 27},
                     {'O', 'P', 'Q', 'R', 'S', 'T', 'U', 0},
                     {'V', 'W', 'X', 'Y', 'Z', '.', '\'', 0} };

struct Directions {
        int horizontal;
        int vertical;
};

struct IndicesOfChar {
        int row;
        int col;
};


struct Directions getDirections(struct IndicesOfChar, struct IndicesOfChar);

void navigateToCharacter(struct Directions, CURL*, int);

struct IndicesOfChar getIndices(char);

void getRowAndColumn(char);

void printIndices(struct IndicesOfChar);

void sendPost(CURL*, char*, char*);

void goToYoutubeKeyboard(CURL*);

void homeOrYoutube(CURL*);

void playingVideo(CURL*, int*);

void interpretKeyboard(CURL*, int*);

void takeInput(CURL*, int*);

void navigateToFirstSuggestion(CURL*, int*);

#endif
