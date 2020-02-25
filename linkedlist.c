#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#include "value.h"
#include "talloc.h"
#include "assert.h"


/*
 * Creates a NULL_TYPE Value
 * returns: NULL_TYPE Value
 */
Value *makeNull() {
    Value *nullValue = talloc(sizeof(Value));
    nullValue->type = NULL_TYPE;
    return nullValue;
}

/*
 * Creates a new CONS_TYPE Value to the front of the list
 * Value *newCar: pointer to the Value struct that we
 * want to add to the list.
 * Value *newCdr: the rest of the list
 * returns: a CONS_TYPE Value
 */
Value *cons(Value *newCar, Value *newCdr) {
    Value *consValue = talloc(sizeof(Value));
    consValue->type = CONS_TYPE;
    consValue->c.car = newCar;
    consValue->c.cdr = newCdr;
    return consValue;
}

/*
 * Displays the contents of the linked list
 * in a readable format
 * Value *list: the linked list of Value structs
 */
void display(Value *list) {
    assert(list != NULL);
    //while (list->type != NULL_TYPE){
    while (isNull(list) == false) {
        switch (list->c.car->type) {
            case INT_TYPE:
                printf("%i\n",list->c.car->i);
                break;
            case DOUBLE_TYPE:
                printf("%f\n",list->c.car->d);
                break;
            case STR_TYPE:
                printf("%s\n",list->c.car->s);
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

/*
 * Reverses the list
 * Value *list: the linked list of Value structs
 * returns: the reversed list
 */
Value *reverse(Value *list){
    assert(list != NULL);
    Value *newList;
    newList = makeNull();
    while (isNull(list) == false) {
        Value *newVal = talloc(sizeof(Value));
        newVal->type = CONS_TYPE;
        newVal->c.car = car(list);
        newVal->c.cdr = newList;
        newList = newVal;
        list = cdr(list);
    }
    return newList;
}

/*
 * Retrieves the first element in the list
 * Value *list: the linked list of Value structs
 * returns: the first element in the list
 */
Value *car(Value *list) {
    assert(list->type != NULL_TYPE);
    assert(list->c.car != NULL);
    return list->c.car;
}

/*
 * Retrieves the list without the first element
 * Value *list: the linked list of Value structs
 * returns: the list without the first element
 */
Value *cdr(Value *list) {
    assert(list->type != NULL_TYPE);
    assert(list->c.cdr != NULL);
    return list->c.cdr;
}

/*
 * Checks if the list is pointing to a NULL_TYPE
 * Value *value: a Value struct
 * returns: true or false, depending on what *value is
 * pointing to
 */
bool isNull(Value *value) {
    assert(value != NULL);
    bool null;
    switch (value->type) {
        case NULL_TYPE:
            null = true;
            break;
        default:
            null = false;
    }
    return null;
}

/*
 * Measures the length of the list
 * Value *value: the linked list of Value structs
 * returns: the length of the list
 */
int length(Value *value) {
    assert(value->type != INT_TYPE);
    assert(value->type != DOUBLE_TYPE);
    assert(value->type != STR_TYPE);
    int len = 0;
    while (isNull(value) == false) {
        switch (value->c.car->type) {
            case INT_TYPE:
                len += 1;
                break;
            case DOUBLE_TYPE:
                len += 1;
                break;
            case STR_TYPE:
                len += 1;
                break;
            case NULL_TYPE:
                break;
            case CONS_TYPE:
                len += 1;
            case SYMBOL_TYPE:
                break;
            case BOOL_TYPE:
                len += 1;
                break;
            default:
                printf("length function error\n");
                break;
        }
        value = cdr(value);
    }
    return len;
}