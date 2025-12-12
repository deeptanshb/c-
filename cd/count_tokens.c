#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 100

// List of C keywords
const char *keywords[] = {"int", "float", "if", "else", "return", "for", "while", "do", "switch", "case", "break", "continue", "char", "double", "void"};
const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

// Function to check if a word is a keyword
int isKeyword(const char *word) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to identify tokens from a file
void tokenizeFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    char ch, buffer[MAX_TOKEN_LEN];
    int i = 0;

    printf("\nTokens found in %s:\n", filename);

    while ((ch = fgetc(file)) != EOF) {
        // Identifiers & keywords
        if (isalpha(ch)) {
            buffer[i++] = ch;
            while (isalnum(ch = fgetc(file)) || ch == '_') {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            i = 0;
            ungetc(ch, file);

            if (isKeyword(buffer)) {
                printf("[Keyword] %s\n", buffer);
            } else {
                printf("[Identifier] %s\n", buffer);
            }
        }
        // Numbers
        else if (isdigit(ch)) {
            buffer[i++] = ch;
            while (isdigit(ch = fgetc(file)) || ch == '.') {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            i = 0;
            ungetc(ch, file);
            printf("[Number] %s\n", buffer);
        }
        // Operators
        else if (strchr("+-*/=><!", ch)) {
            buffer[i++] = ch;
            ch = fgetc(file);
            if (strchr("=+-*/", ch)) { // Checking for compound operators like '+=', '=='
                buffer[i++] = ch;
            } else {
                ungetc(ch, file);
            }
            buffer[i] = '\0';
            i = 0;
            printf("[Operator] %s\n", buffer);
        }
        // Symbols
        else if (strchr("(){}[],;\"", ch)) {
            printf("[Symbol] %c\n", ch);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    tokenizeFile(argv[1]);

    return 0;
}

