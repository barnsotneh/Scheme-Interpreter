#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#include "value.h"
#include "talloc.h"
#include "assert.h"
#include "tokenizer.h"

FILE *inputFile = NULL;

/*
 * Helper function. Defines the type of a character
 * and returns it.
 */
valueType defineType(char character) {
    valueType type = NULL_TYPE;
    if (character == '0' || character == '1' || character == '2' || character == '3' || character == '4' || character == '5'
        || character == '6' || character == '7' || character == '8' || character == '9') {
        type = INT_TYPE;
    } else if (character == '.') {
        type = DOUBLE_TYPE;
    } else if ( character == '\'' || character == '!' || character == '$' || character == '%' || character == '&'
               || character == '*' || character == '\\' || character == ':' || character == '<' || character == '>' || character == '=' || character == '?'
               || character == '-' || character == '_' || character == '^' || character == '+' || character == '-' || character == '.' || character == 'a' || character == 'b' ||
               character == 'c' || character == 'd' || character == 'e' || character == 'f'|| character == 'g' || character == 'g' ||
               character == 'h' || character == 'i' || character == 'j' || character == 'k' || character == 'l' || character == 'm' || character == 'n'
               || character == 'o' || character == 'p' || character == 'q' || character == 'r' || character == 's' || character == 't' || character == 'u'
               || character == 'v' || character == 'w'|| character == 'x' || character == 'y' || character == 'z' || character == 'A' ||
               character == 'B' || character == 'C' || character == 'D' || character == 'E' || character == 'F' || character == 'G' || character == 'H' || character == 'I' ||
               character == 'J' || character == 'K' || character == 'L' || character == 'M' || character == 'N' || character == 'O'
               || character == 'P' || character == 'Q' || character == 'R' || character == 'S' || character == 'T' || character == 'U' || character == 'V'
               || character == 'W' || character == 'X'|| character == 'Y' || character == 'Z' || character == '/' ) {
        type = SYMBOL_TYPE;
    }
    return type;
}

/*Helper function. creates cons cell with the car pointing to given type
 * and token and the cdr pointing to the
 *next cons cell in the list and returns it
*/
Value *add(Value *list, char type, char *token) {
    Value *newVal = talloc(sizeof(Value));
    newVal->type = type;
    switch (type) {
        case INT_TYPE:
            newVal->i = (int)strtol(token, NULL, 10);
            break;
        case DOUBLE_TYPE:
            newVal->d = strtod(token, NULL);
            break;
        case STR_TYPE:
            newVal->s = token;
            break;
        case OPEN_TYPE:
            break;
        case CLOSE_TYPE:
            break;
        case BOOL_TYPE:
            if (*token == 'f') {
                newVal->i = 0;
            } else {
                newVal->i = 1;
            }
            break;
        case SYMBOL_TYPE:
            newVal->s = token;
            break;
        default:
            printf("add default case error\n");
    }
    Value *valBox = cons(newVal, list);
    return valBox;
}

/*
 * Tokenizes the DrRacket input file
 */
Value *tokenize(char *inputFileName) {
    inputFile = fopen(inputFileName, "r");
    char charRead;
    Value *list = makeNull();
    charRead = (char) fgetc(inputFile);
    char *token = "";
    char charArray[1000];
    char numArray[1000];
    char symbolArray[1000];
    char type;
    while (charRead != EOF) {
        if (charRead == ';') {          //comment case
            while (charRead != '\n' && charRead != EOF) {
                charRead = (char) fgetc(inputFile);
            }
            charRead = (char) fgetc(inputFile);
        }
        if (charRead == '\"') {         //string case
            int counter = 0;
            charArray[counter] = '\"';
            counter++;
            charRead = (char) fgetc(inputFile);
            while (charRead != '\"') {
                charArray[counter] = charRead;
                charRead = (char) fgetc(inputFile);
                counter++;
            }
            charArray[counter] = '\"';
            counter++;
            charArray[counter] = '\0';
            counter++;
            char *strToken = talloc(sizeof(char) * counter); //copy the charArray over to the char * variable
            for (int i = 0; i < counter; i++) {
                strToken[i] = charArray[i];
            }
            type = STR_TYPE;
            list = add(list, type, strToken);
        } else if (charRead == '(') {       //case of the open paren
//            if (strlen(token) != 0) {
//                printf("shouldn't be here");
//                type = OPEN_TYPE;
//                list = add(list, type, token);
//            }
            type = OPEN_TYPE;
            token = "(";
            list = add(list, type, token);
            token = "";
        } else if (charRead == ')') {       //case of the closed paren
            if (strlen(token) != 0) {
                type = CLOSE_TYPE;
                list = add(list, type, token);
            }
            type = CLOSE_TYPE;
            token = ")";
            list = add(list, type, token);
            token = "";
        } else if (charRead == '#') {        //boolean case
            charRead = (char) fgetc(inputFile);
            if (charRead == 'f' || charRead == 't') {
                type = BOOL_TYPE;
                list = add(list, type, &charRead);
            }
        }else {     //integer, double, and symbol case
            int numCount = 0;
            type = defineType(charRead);
            if (charRead == '+' || charRead == '-'){ //special check to handle signed integers
                char temp = charRead;
                charRead = (char) fgetc(inputFile);
                if (defineType(charRead) == INT_TYPE || defineType(charRead) == DOUBLE_TYPE) {
                    numCount++;
                    if (defineType(charRead) == INT_TYPE) {
                        type = INT_TYPE;
                    } else {
                        type = DOUBLE_TYPE;
                    }
                    numArray[0] = temp;
                } else {
                    ungetc(charRead, inputFile);
                    charRead = temp;
                }
            }
            if (type == INT_TYPE) {  //integer case
                numArray[numCount] = charRead;
                numCount++;
                charRead = (char) fgetc(inputFile);
                type = defineType(charRead);
                if (type == SYMBOL_TYPE) {   //checking for invalid symbol - can't have a number followed by symbol
                    printf("digit followed by symbol");
                    texit(1);
                }
                while (type == INT_TYPE) { //checks for more integers after the initial
                    numArray[numCount] = charRead;
                    numCount++;
                    charRead = (char) fgetc(inputFile);
                    type = defineType(charRead);
                    if (type == SYMBOL_TYPE) {
                        printf("digit followed by symbol");
                        texit(1);
                    }
                }
                if (type == DOUBLE_TYPE) {
                    //if the type becomes double type,
                    //add those the decimal and the numbers after the decimal
                    //to the numArray - determined by the presence of a decimal
                    numArray[numCount] = charRead;
                    numCount++;
                    charRead = (char) fgetc(inputFile);
                    type = defineType(charRead);
                    if (type == DOUBLE_TYPE || type == STR_TYPE || type == BOOL_TYPE) {
                        printf("two consecutive decimals");
                        texit(1);
                    }
                    while (type == INT_TYPE) {
                        numArray[numCount] = charRead;
                        numCount++;
                        charRead = (char) fgetc(inputFile);
                        type = defineType(charRead);
                        if (type == DOUBLE_TYPE || type == STR_TYPE || type == BOOL_TYPE) {
                            printf("too many decimals");
                            texit(1);
                        }
                    }
                    char *numToken = talloc(sizeof(char) * (numCount + 1));
                    for (int i = 0; i < numCount; i++) {
                        numToken[i] = numArray[i];
                    }
                    numToken[numCount] = '\0';
                    type = DOUBLE_TYPE;
                    list = add(list, type, numToken);
                    if (charRead == ')') {
                        type = CLOSE_TYPE;
                        token = ")";
                        list = add(list, type, token);
                        token = "";
                    } else if (charRead == '(') {
                        type = OPEN_TYPE;
                        token = "(";
                        list = add(list, type, token);
                        token = "";
                    }
                } else {
                    char *numToken = talloc(sizeof(char) * (numCount + 1));
                    for (int i = 0; i < numCount; i++) {
                        numToken[i] = numArray[i];
                    }
                    numToken[numCount] = '\0';
                    type = INT_TYPE;
                    list = add(list, type, numToken);
                    if (charRead == ')') {
                        type = CLOSE_TYPE;
                        token = ")";
                        list = add(list, type, token);
                        token = "";
                    } else if (charRead == '(') {
                        type = OPEN_TYPE;
                        token = "(";
                        list = add(list, type, token);
                        token = "";
                    }
                }
            } else if (type == DOUBLE_TYPE) {
                //if it starts in a decimal, add the point and the numbers after it
                //to the numArray
                numArray[numCount] = charRead;
                numCount++;
                charRead = (char) fgetc(inputFile);
                type = defineType(charRead);
                if (type == DOUBLE_TYPE || type == STR_TYPE || type == BOOL_TYPE) {
                    printf("starting decimal followed by decimal");
                    texit(1);
                }
                while (type == INT_TYPE) { //gets the numbers after the decimal point
                    numArray[numCount] = charRead;
                    numCount++;
                    charRead = (char) fgetc(inputFile);
                    type = defineType(charRead);
                    if (type == DOUBLE_TYPE || type == STR_TYPE || type == BOOL_TYPE) {
                        printf("decimal followed by non-digit");
                        texit(1);
                    }
                }
                char *numToken = talloc(sizeof(char) * (numCount + 1));
                for (int i = 0; i < numCount; i++) {
                    numToken[i] = numArray[i];
                }
                numToken[numCount] = '\0';
                type = DOUBLE_TYPE;
                list = add(list, type, numToken);
                if (charRead == ')') {
                    type = CLOSE_TYPE;
                    token = ")";
                    list = add(list, type, token);
                    token = "";
                } else if (charRead == '(') {
                    type = OPEN_TYPE;
                    token = "(";
                    list = add(list, type, token);
                    token = "";
                }
            } else if (type == SYMBOL_TYPE) {       //symbol case
                int symCount = 0;
                int charReadCount = 0;
                symbolArray[symCount] = charRead;
                symCount++;
                charRead = (char) fgetc(inputFile);
                type = defineType(charRead);
                while (type == SYMBOL_TYPE || type == INT_TYPE || type == DOUBLE_TYPE) {
                    symbolArray[symCount] = charRead;
                    symCount++;
                    charRead = (char) fgetc(inputFile);
                    type = defineType(charRead);
                }
                char *symToken = talloc(sizeof(char) * (symCount + 1));
                for (int i = 0; i < symCount; i++) {
                    symToken[i] = symbolArray[i];
                }
                symToken[symCount] = '\0';
                type = SYMBOL_TYPE;
                list = add(list, type, symToken);
                if (charRead == ')') {
                    type = CLOSE_TYPE;
                    token = ")";
                    list = add(list, type, token);
                    token = "";
                }
                if (charRead == '(') {
                    type = OPEN_TYPE;
                    token = "(";
                    list = add(list, type, token);
                    token = "";
                }
            }
        }
        charRead = (char) fgetc(inputFile);
    }
    fclose(inputFile);
    Value *revList = reverse(list);
    return revList;
}

/* displays tokens in the format of token:type */
void displayTokens(Value *list) {
    assert(list != NULL);
    while (isNull(list) == false) {
        switch (list->c.car->type) {
            case INT_TYPE:
                printf("%i:integer\n", list->c.car->i);
                break;
            case DOUBLE_TYPE:
                printf("%f:float\n", list->c.car->d);
                break;
            case STR_TYPE:
                printf("%s:string\n", list->c.car->s);
                break;
            case OPEN_TYPE:
                printf("(:open\n");
                break;
            case CLOSE_TYPE:
                printf("):close\n");
                break;
            case BOOL_TYPE:
                if (list->c.car->i == 1) {
                    printf("#t:boolean\n");
                } else {
                    printf("#f:boolean\n");
                }
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol\n", list->c.car->s);
                break;
            case CONS_TYPE:
                printf("display in cons_type\n");
                break;
            case NULL_TYPE:
                printf("display in null_type\n");
                break;
            default:
                printf("display default case error\n");
        }
        list = cdr(list);
    }
}
