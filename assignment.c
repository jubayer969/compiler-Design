#include <stdio.h>
#include <string.h>

#define MAX_TOKENS 100
#define MAX_SYMBOLS 20


typedef struct {
    char type[10];
    char value[20];
} Token;

typedef struct {
    int id_num;
    char name[20];
    char datatype[10];
    char category[10];
    char scope[20];
} Symbol;


Token tokens[] = {
    {"kw", "float"}, {"id", "x1"}, {"op", "="}, {"num", "3.125"}, {"sep", ";"},
    {"kw", "double"}, {"id", "f1"}, {"par", "("}, {"kw", "int"}, {"id", "x"}, {"par", ")"}, {"brc", "{"},
    {"kw", "double"}, {"id", "z"}, {"sep", ";"}, {"id", "z"}, {"op", "="}, {"num", "0.01"}, {"sep", ";"},
    {"kw", "return"}, {"id", "z"}, {"sep", ";"}, {"brc", "}"},
    {"kw", "int"}, {"id", "main"}, {"par", "("}, {"kw", "void"}, {"par", ")"}, {"brc", "{"},
    {"kw", "int"}, {"id", "n1"}, {"sep", ";"}, {"kw", "double"}, {"id", "z"}, {"sep", ";"},
    {"id", "n1"}, {"op", "="}, {"num", "25"}, {"sep", ";"},
    {"id", "z"}, {"op", "="}, {"id", "f1"}, {"par", "("}, {"id", "n1"}, {"par", ")"}, {"sep", ";"}
};

int tokenCount = sizeof(tokens) / sizeof(tokens[0]);


Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;


int findSymbol(char *name, char *scope) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 && strcmp(symbolTable[i].scope, scope) == 0) {
            return symbolTable[i].id_num;
        }
    }
    return -1;
}


void addSymbol(char *name, char *datatype, char *category, char *scope) {
    if (findSymbol(name, scope) == -1) {
        symbolTable[symbolCount].id_num = symbolCount + 1;
        strcpy(symbolTable[symbolCount].name, name);
        strcpy(symbolTable[symbolCount].datatype, datatype);
        strcpy(symbolTable[symbolCount].category, category);
        strcpy(symbolTable[symbolCount].scope, scope);
        symbolCount++;
    }
}


void processTokens() {
    char currentScope[20] = "global";
    char lastType[10] = "";

    for (int i = 0; i < tokenCount; i++) {

        if (strcmp(tokens[i].type, "kw") == 0) {
            strcpy(lastType, tokens[i].value);
        }

        else if (strcmp(tokens[i].type, "id") == 0) {


            if (strcmp(tokens[i+1].type, "par") == 0) {
                addSymbol(tokens[i].value, lastType, "function", "global");
                strcpy(currentScope, tokens[i].value);
            }

            else if (strcmp(tokens[i-1].type, "kw") == 0) {
                addSymbol(tokens[i].value, lastType, "variable", currentScope);
            }

            else {
                addSymbol(tokens[i].value, "", "usage", currentScope);
            }
        }

        else if (strcmp(tokens[i].type, "brc") == 0 && strcmp(tokens[i].value, "}") == 0) {
            strcpy(currentScope, "global");
        }
    }
}


void printSymbolTable() {
    printf("\nSymbol Table:\n");
    printf("ID\tName\tType\tCategory\tScope\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("id#%d\t%s\t%s\t%s\t\t%s\n",
            symbolTable[i].id_num,
            symbolTable[i].name,
            symbolTable[i].datatype,
            symbolTable[i].category,
            symbolTable[i].scope);
    }
}


void printModifiedStream() {
    printf("\nModified Token Stream:\n");
    char currentScope[20] = "global";

    for (int i = 0; i < tokenCount; i++) {
        if (strcmp(tokens[i].type, "id") == 0) {
            int id = findSymbol(tokens[i].value, currentScope);
            if (id == -1) id = findSymbol(tokens[i].value, "global");
            printf("[id#%d] ", id);
        } else {
            printf("[%s %s] ", tokens[i].type, tokens[i].value);
        }

        if (strcmp(tokens[i].type, "id") == 0 && strcmp(tokens[i+1].type, "par") == 0) {
            strcpy(currentScope, tokens[i].value);
        }
        if (strcmp(tokens[i].type, "brc") == 0 && strcmp(tokens[i].value, "}") == 0) {
            strcpy(currentScope, "global");
        }
    }
    printf("\n");
}


int main() {
    processTokens();
    printSymbolTable();
    printModifiedStream();
    return 0;
}
