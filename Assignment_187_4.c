
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES       2000
#define MAX_TOKENS      200
#define MAX_TOKEN_LEN   64
#define MAX_SYMBOLS     1000


typedef struct {
    char name[MAX_TOKEN_LEN];
    int  lineDeclared;
} Symbol;


static Symbol symbolTable[MAX_SYMBOLS];
static int    symbolCount = 0;
static char   stackBuf[MAX_LINES * MAX_TOKENS];
static int    top = -1;


void push(char ch) { stackBuf[++top] = ch; }
char pop()     { return (top<0)? '\0' : stackBuf[top--]; }


void reportError(const char *msg, int line) {
    printf("Line %d: Error - %s\n", line, msg);
}


int isDeclKeyword(const char *tok) {
    return !strcmp(tok, "int")
        || !strcmp(tok, "float")
        || !strcmp(tok, "double")
        || !strcmp(tok, "char")
        || !strcmp(tok, "void");
}


int isKeyword(const char *tok) {
    static const char *kw[] = {
        "int","float","double","char","void",
        "if","else","return","for","while"
    };
    for (int i = 0; i < (int)(sizeof(kw)/sizeof(*kw)); i++)
        if (!strcmp(tok, kw[i]))
            return 1;
    return 0;
}


int lookupSymbol(const char *id) {
    for (int i = 0; i < symbolCount; i++)
        if (!strcmp(symbolTable[i].name, id))
            return symbolTable[i].lineDeclared;
    return -1;
}


void declareId(const char *id, int line) {
    int prev = lookupSymbol(id);
    if (prev != -1) {
        char buf[128];
        snprintf(buf, sizeof(buf),
                 "Duplicate declaration of '%s' (already declared at line %d)",
                 id, prev);
        reportError(buf, line);
    } else if (symbolCount < MAX_SYMBOLS) {
        strcpy(symbolTable[symbolCount].name, id);
        symbolTable[symbolCount].lineDeclared = line;
        symbolCount++;
    }
}


void tokenizeLine(const char *line, char tokens[][MAX_TOKEN_LEN], int *tkCount) {
    int i = 0, n = strlen(line), t = 0;

    *tkCount = 0;
    while (i < n) {
        if (isspace((unsigned char)line[i])) {
            i++;
        }
        else if (strchr("(){};,<>+-=*/", line[i])) {

            tokens[*tkCount][0] = line[i];
            tokens[*tkCount][1] = '\0';
            (*tkCount)++;
            i++;
        }
        else {

            int j = 0;
            while (i < n &&
                   (isalnum((unsigned char)line[i]) || line[i]=='_' || line[i]=='.')) {
                if (j < MAX_TOKEN_LEN-1)
                    tokens[*tkCount][j++] = line[i];
                i++;
            }
            tokens[*tkCount][j] = '\0';
            (*tkCount)++;
        }
    }
}


void printIntermediate(FILE *fp) {
    char   lineBuf[1024];
    int    lineNo = 0;
    printf("--- Intermediate Output ---\n");

    rewind(fp);
    while (fgets(lineBuf, sizeof(lineBuf), fp)) {
        lineNo++;

        char *trim = lineBuf;
        while (isspace((unsigned char)*trim)) trim++;
        if (strncmp(trim, "/*", 2)==0 || strncmp(trim, "//", 2)==0) {
            printf("%2d %s", lineNo, lineBuf);
            continue;
        }

        char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
        int  tkCount = 0;
        tokenizeLine(lineBuf, tokens, &tkCount);

        if (tkCount == 0) {
            printf("%2d\n", lineNo);
        } else {
            printf("%2d", lineNo);
            for (int i = 0; i < tkCount; i++) {
                if (isKeyword(tokens[i])) {
                    printf(" kw %s", tokens[i]);
                }
                else if (isdigit((unsigned char)tokens[i][0])) {
                    printf(" num %s", tokens[i]);
                }
                else {
                    printf(" id %s", tokens[i]);
                }
            }
            printf("\n");
        }
    }
    printf("\n");
}


void checkSyntax(FILE *fp) {
    char   lineBuf[1024];
    int    lineNo = 0;
    int    ifCount = 0;

    rewind(fp);
    while (fgets(lineBuf, sizeof(lineBuf), fp)) {
        lineNo++;
        char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
        int  tkCount = 0;

        tokenizeLine(lineBuf, tokens, &tkCount);


        for (int i = 0; i < tkCount; i++) {
            for (int j = i + 1; j < tkCount; j++) {
                if (!strcmp(tokens[i], tokens[j]) &&
                    strcmp(tokens[i], "(") && strcmp(tokens[i], ")") &&
                    strcmp(tokens[i], "{") && strcmp(tokens[i], "}") &&
                    strcmp(tokens[i], ";")) {
                    reportError("Duplicate token", lineNo);
                    goto next_line;
                }
            }
        }


        for (int i = 0; lineBuf[i]; i++) {
            if (lineBuf[i] == '(' || lineBuf[i] == '{') {
                push(lineBuf[i]);
            }
            else if (lineBuf[i] == ')') {
                if (pop() != '(')
                    reportError("Unbalanced parentheses", lineNo);
            }
            else if (lineBuf[i] == '}') {
                if (pop() != '{')
                    reportError("Unbalanced braces", lineNo);
            }
        }


        for (int i = 0; i < tkCount; i++) {
            if (!strcmp(tokens[i], "if")) {
                ifCount++;
            }
            else if (!strcmp(tokens[i], "else")) {
                if (ifCount == 0)
                    reportError("Unmatched 'else'", lineNo);
                else
                    ifCount--;
            }
        }


        for (int i = 0; i + 1 < tkCount; i++) {
            if (isDeclKeyword(tokens[i])) {
                declareId(tokens[i + 1], lineNo);
            }
        }

    next_line:;
    }


    while (top >= 0) {
        char ch = pop();
        if (ch == '(')
            reportError("Missing closing ')'", lineNo);
        else if (ch == '{')
            reportError("Missing closing '}'", lineNo);
    }
}

int main(void) {
    FILE *fp = fopen("sampleinput.c", "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open sampleinput.c\n");
        return 1;
    }

    printIntermediate(fp);

    printf("--- Detected Errors ---\n");
    checkSyntax(fp);

    fclose(fp);
    return 0;
}
