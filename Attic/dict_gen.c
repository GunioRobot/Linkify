#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUMBER_ARGUMENT "n"
#define WORD_ARGUMENT "w"


static void usage(void) {
    puts("Usage:");
    printf("  %s <minimum length> <maximum length>\n", NUMBER_ARGUMENT);
    printf("  %s <initial word> <maximum length>\n", WORD_ARGUMENT);
}


int main(int argc, char** argv) {
    size_t length = SIZE_MAX;
    size_t max_length = 0;
    char* initial;
    char* word;
    
    if (argc != 4) {
        usage();
        return EXIT_FAILURE;
    }
    
    if (strcmp(argv[1], NUMBER_ARGUMENT) == 0) {
        length = strtoul(argv[2], NULL, 10);
        max_length = strtoul(argv[3], NULL, 10);
        initial = NULL;
    }
    else if (strcmp(argv[1], WORD_ARGUMENT) == 0) {
        length = strlen(argv[2]);
        max_length = strtoul(argv[3], NULL, 10);
        initial = argv[2];
    }
    
    if (length > max_length) {
        usage();
        return EXIT_FAILURE;
    }
    
    word = malloc((max_length + 1) * sizeof(char));
    
    if (word == NULL) {
        perror(NULL);
        return EXIT_FAILURE;
    }
    
    if (initial == NULL) {
        for (initial = word + max_length; initial != word; --initial) {
            *initial = CHAR_MIN;
        }
    }
    else {
        strcpy(word, argv[2]);
    }
    
    while (length <= max_length) {
        size_t i;
        word[length] = '\0';
    
    NEXT_WORD:
        fwrite(word, sizeof(char), length, stdout);
        putchar('\n');
        
        for (i = 0; i < length; ++i) {
            if (word[i]++ < CHAR_MAX) {
                goto NEXT_WORD;
            }
            if (word[i] > CHAR_MAX) {
                word[i] = CHAR_MIN;
            }
        }
        
        word[length++] = CHAR_MIN;
    }
    
    free(word);
    return EXIT_SUCCESS;
}
