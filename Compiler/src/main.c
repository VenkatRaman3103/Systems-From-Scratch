#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

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

TokenLiteral generat_number(char curr, FILE *file) {
    TokenLiteral token;
    token.type = INT;

    int val = 0;

    while (isdigit(curr) && curr != EOF) {
        if (!isdigit(curr)) {
            break;
        }

        val = (val * 10) + (curr - '0');
        printf("%d\n", (curr - '0'));

        curr = fgetc(file);
    }

    ungetc(curr, file);

    token.value = val;

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
            TokenLiteral test_token = generat_number(curr, file);
            printf("test_toke: %d\n", test_token.value);
        } else if (isalpha(curr)) {
            printf("found CHARACTER: %c\n", curr);
        }
        curr = fgetc(file);
    }

    fclose(file);
}

int main() {

    FILE *file;

    file = fopen("test.unn", "r");

    if (file == NULL) {
        printf("Failed to open file\n");
    }

    lexer(file);

    return 0;
}
