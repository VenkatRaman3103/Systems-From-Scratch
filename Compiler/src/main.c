#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SEMI,
    OPEN_PAREN,
    CLOSE_PAREN,
} TypeSeparator;

typedef enum {
    EXIT,
} TypeKeyword;

typedef enum {
    INT,
} TypeLiteral;

typedef struct {
    TypeSeparator type;
} TokenSeparator;

typedef struct {
    TypeKeyword type;
} TokenKeyword;

typedef struct {
    TypeLiteral type;
    int value;
} TokenLiteral;

typedef struct {

} Token;

// generating token
// numbers

TokenKeyword *generat_keyword(char curr, FILE *file) {
    TokenKeyword *token = malloc(sizeof(TokenKeyword));
    char *keyword = malloc(sizeof(char) * 4);

    int keyword_index = 0;

    while (isalpha(curr) && curr != EOF) {
        keyword[keyword_index] = curr;
        curr = fgetc(file);
    }

    if (strcmp(keyword, "exit")) {
        token->type = EXIT;
    }

    return token;
}

TokenLiteral *generat_number(int curr, FILE *file) {
    TokenLiteral *token = malloc(sizeof(TokenLiteral));
    token->type = INT;

    int val = 0;

    while (isdigit(curr) && curr != EOF) {
        if (curr == '(') {
            printf("found open paren\n");

            break;
        }

        val = (val * 10) + (curr - '0');
        curr = fgetc(file);
    }

    ungetc(curr, file);

    token->value = val;

    return token;
}

void lexer(FILE *file) {

    int curr = fgetc(file);

    while (curr != EOF) {
        if (curr == ';') {
            printf("found ;\n");
        } else if (curr == '(') {
            printf("found OPEN_PAREN\n");
        } else if (curr == ')') {
            printf("found CLOSE_PAREN\n");
        } else if (isdigit(curr)) {
            TokenLiteral *test_token = generat_number(curr, file);
            printf("test_toke: %d\n", test_token->value);
        } else if (isalpha(curr)) {
            TokenKeyword *test_keywod = generat_keyword(curr, file);
            /* printf("found CHARACTER: %s\n", *test_keywod); */
        }
        curr = fgetc(file);
    }

    fclose(file);
}

int main() {

    FILE *file;

    file = fopen("test.lg", "r");

    if (file == NULL) {
        printf("Failed to open file\n");
    }

    lexer(file);

    return 0;
}
