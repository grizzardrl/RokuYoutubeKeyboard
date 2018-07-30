#ifndef ROKU_HEADER
#define ROKU_HEADER

#include <curl/curl.h>

static const char chars[4][8];

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
