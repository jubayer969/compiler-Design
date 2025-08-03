#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


const char *keywords[] = {
    "char", "int", "float", "if", "else", "while", "for", "return", "void"
};


int is_keyword(const char *word) {
    for (int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }
    return 0;
}


int is_identifier(const char *word) {
    if (!isalpha(word[0]) && word[0] != '_') return 0;
    for (int i = 1; word[i]; i++) {
        if (!isalnum(word[i]) && word[i] != '_')
            return 0;
    }
    return 1;
}


int is_integer(const char *word) {
    for (int i = 0; word[i]; i++) {
        if (!isdigit(word[i]))
            return 0;
    }
    return 1;
}


int is_float(const char *word) {
    int dot_count = 0;
    for (int i = 0; word[i]; i++) {
        if (word[i] == '.') {
            dot_count++;
        } else if (!isdigit(word[i])) {
            return 0;
        }
    }
    return dot_count == 1;
}


int is_operator(const char *op) {
    const char *ops[] = {"+", "-", "*", "/", "=", "<=", ">=", "==", "!=", "<", ">"};
    for (int i = 0; i < sizeof(ops)/sizeof(ops[0]); i++) {
        if (strcmp(op, ops[i]) == 0)
            return 1;
    }
    return 0;
}


int is_separator(char c) {
    return (c == ';' || c == ',' || c == '\'');
}


int is_parenthesis(char c) {
    return (c == '(' || c == ')');
}


void print_token(FILE *out, const char *lexeme) {
    if (is_keyword(lexeme)) {
        fprintf(out, "[kw %s] ", lexeme);
    } else if (is_operator(lexeme)) {
        fprintf(out, "[op %s] ", lexeme);
    } else if (is_separator(lexeme[0]) && lexeme[1] == '\0') {
        fprintf(out, "[sep %c] ", lexeme[0]);
    } else if (is_parenthesis(lexeme[0]) && lexeme[1] == '\0') {
        fprintf(out, "[par %c] ", lexeme[0]);
    } else if (is_float(lexeme)) {
        fprintf(out, "[num %s] ", lexeme);
    } else if (is_integer(lexeme)) {
        fprintf(out, "[num %s] ", lexeme);
    } else if (is_identifier(lexeme)) {
        fprintf(out, "[id %s] ", lexeme);
    } else {
        fprintf(out, "[unkn %s] ", lexeme);
    }
}

int main() {
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        printf("Error: input.txt not found.\n");
        return 1;
    }

    FILE *out = fopen("output.txt", "w");
    if (!out) {
        printf("Error: Cannot create output.txt\n");
        fclose(fp);
        return 1;
    }

    char ch, word[100];
    int i = 0;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '<' || ch == '>' || ch == '=' || ch == '!') {
            char next = fgetc(fp);
            if (next == '=') {
                char op[3] = {ch, next, '\0'};
                print_token(out, op);
            } else {
                ungetc(next, fp);
                char op[2] = {ch, '\0'};
                print_token(out, op);
            }
        }
        else if (is_operator((char[]){ch, '\0'})) {
            char op[2] = {ch, '\0'};
            print_token(out, op);
        }
        else if (is_separator(ch) || is_parenthesis(ch)) {
            char sep[2] = {ch, '\0'};
            print_token(out, sep);
        }
        else if (isalnum(ch) || ch == '_' || ch == '.') {
            word[i++] = ch;
        }
        else {
            if (i > 0) {
                word[i] = '\0';
                print_token(out, word);
                i = 0;
            }
        }
    }


    if (i > 0) {
        word[i] = '\0';
        print_token(out, word);
    }

    fclose(fp);
    fclose(out);

    printf("Lexical analysis complete. Tokens written to output.txt\n");
    return 0;
}
