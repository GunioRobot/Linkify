#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_VALUE CHAR_MAX
#define MIN_VALUE CHAR_MIN

#define NUMBER_ARGUMENT "n"
#define WORD_ARGUMENT "w"


static size_t parse(char* string) {
    char* invalid = NULL;
    unsigned long int number = strtoul(string, &invalid, 10);
    
    errno = ((*string != '\0') && (*invalid == '\0')) ? 0 : EINVAL;
    return number;
}


static int usage(void) {
    puts("Dictionary generator.");
    puts("Usage:");
    printf("  %s <minimum length> <maximum length>\n", NUMBER_ARGUMENT);
    printf("  %s <initial word> <maximum length>\n", WORD_ARGUMENT);
    return EXIT_FAILURE;
}


int main(int argc, char** argv) {
    size_t length = ((size_t) ~0);
    size_t max_length = 0;
    char* initial;
    char* word;
    
    if (argc != 4) {
        return usage();
    }
    
    if (strcmp(argv[1], NUMBER_ARGUMENT) == 0) {
        length = parse(argv[2]);
        if (errno == EINVAL) {
            return usage();
        }
        
        max_length = parse(argv[3]);
        if (errno == EINVAL) {
            return usage();
        }
        
        initial = NULL;
    }
    else if (strcmp(argv[1], WORD_ARGUMENT) == 0) {
        length = strlen(argv[2]);
        max_length = parse(argv[3]);
        initial = argv[2];
        
        if (errno == EINVAL) {
            return usage();
        }
    }
    
    if (length > max_length) {
        return usage();
    }
    
    word = malloc((max_length + 1) * sizeof(char));
    
    if (word == NULL) {
        perror(NULL);
        return EXIT_FAILURE;
    }
    
    if (initial == NULL) {
        memset(word, MIN_VALUE, (max_length + 1) * sizeof(char));
    }
    else {
        strcpy(word, initial);
    }
    
    while (length <= max_length) {
        size_t i;
        word[length] = '\0';
    
    NEXT_WORD:
        fwrite(word, sizeof(char), length, stdout);
        putchar('\n');
        
        for (i = 0; i < length; ++i) {
            if (word[i]++ < MAX_VALUE) {
                goto NEXT_WORD;
            }
            if (word[i] > MAX_VALUE) {
                word[i] = MIN_VALUE;
            }
        }
        
        word[length++] = MIN_VALUE;
    }
    
    free(word);
    return EXIT_SUCCESS;
}
