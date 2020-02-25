#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#include "value.h"
#include "talloc.h"
#include "assert.h"
#include "interpreter.h"
#include "parser.h"
#include "tokenizer.h"

//Global variable for talloc list
Value *activeList = NULL;


/*create a value thats a pointer type and set temp to it */
Value *newCons(Value *temp) {
    Value *addNew = malloc(sizeof(Value));
    addNew->type = PTR_TYPE;
    addNew->p = temp;
    Value *consValue = malloc(sizeof(Value));
    consValue->type = CONS_TYPE;
    consValue->c.car = addNew;
    consValue->c.cdr = activeList;
    return consValue;
}


/* Replacement for malloc that stores the pointers allocated. It should store
* the pointers in some kind of list; a linked list would do fine, but insert
*here whatever code you'll need to do so; don't call functions in the
* pre-existing linkedlist.h.
*/
void *talloc(size_t size) {
    Value *temp = malloc(size);
    activeList = newCons(temp);
    return temp;
}


/* Free all pointers allocated by talloc, as well as whatever memory you
* allocated in lists to hold those pointers.
*/
void tfree() {
    assert(activeList != NULL);
    Value *nextBox;
    nextBox = activeList->c.cdr;
    while(nextBox != NULL) {
        free(activeList->c.car->p);
        free(activeList->c.car);
        free(activeList);
        activeList = nextBox;
        nextBox = nextBox->c.cdr;
    }
    free(activeList->c.car->p);
    free(activeList->c.car);
    free(activeList);
}

/*Replacement for the C function "exit", that consists of two lines: it calls
* tfree before calling exit.
 */
void texit(int status) {
    tfree();
    exit(status);
}
