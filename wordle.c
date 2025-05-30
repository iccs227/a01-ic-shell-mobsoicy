#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

const char *word_list[] = {
    "apple", "grape", "peach", "mango", "berry",
    "lemon", "melon", "cherry", "plums", "guava"
};

const int num_word = sizeof(word_list) / sizeof(word_list[0]);
void guess_checker(const char *guess, const char *target) {
    int correct_guess[WORD_LENGTH] = {0};

    // for green result (correct letter and position)
    for (int i=0; i<WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            printf("\033[1;32m%c\033[0m", guess[i]); // green
            correct_guess[i] = 1;
        }
        else if (guess[i] != target[i]) {
            int found_letter = 0;
            for (int j=0; j<WORD_LENGTH; j++) {
                // check if letter is in target and not already guessed correctly
                if (guess[i] == target[j] && !correct_guess[j]) {
                    found_letter = 1;
                    correct_guess[j] = 1; // mark as found
                    break;
                }
            }
            if (found_letter) {
                printf("\033[1;33m%c\033[0m", guess[i]); // yellow
            } 
            else {
                printf("\033[1;30m%c\033[0m", guess[i]); // gray
            }
        }
        else {
            printf("\033[1;30m%c\033[0m", guess[i]); // gray
        }
    }
    printf("\n");
}

void wordle() {
    const char *target = word_list[rand() % num_word];
    printf("Wlecome to Wordle!\n");
    printf("Guess the %d-letter word. You have %d attempts.\n", WORD_LENGTH, MAX_ATTEMPTS);

    char guess[WORD_LENGTH+1];
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        printf("Attempt %d out of %d: ", attempts+1, MAX_ATTEMPTS);
        scanf("%5s", guess);

        if (strlen(guess) != WORD_LENGTH) {
            printf("Invalid guess.\n");
            continue;
        }

        if (strcmp(guess, target) == 0) {
            printf("Congratulations! You've guessed the word: %s\n", target);
            return;
        }

        guess_checker(guess, target);
        attempts++;
    }
    printf("Sorry, you lose\n");
}