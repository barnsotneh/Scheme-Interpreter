#include <stdio.h>
#include <string.h>
//#include <jmorecfg.h>
#include <stdbool.h>
#include "value.h"
#include "interpreter.h"
#include "tokenizer.h"
#include "talloc.h"
#include "assert.h"
#include "linkedlist.h"
#include "parser.h"
// A frame is a linked list of bindings, and a pointer to another frame.  A
// binding is a variable name (represented as a string), and a pointer to the
// Value it is bound to. Specifically how you implement the list of bindings is
// up to you.
Frame *globalFrame;


//gracefully exits the program if an evaluation error has occurred.
void evaluationError() {
    printf("Evaluation error has occurred\n");
    texit(1);
}


//Prints the result of the eval
void printResult(Value *result) {
        switch (result->type) {
            case INT_TYPE: {
                printf("%i\n", result->i);
                break;
            }
            case BOOL_TYPE: {
                if (result->i == 1) {
                    printf("#t\n");
                } else {
                    printf("#f\n");
                }
                break;
            }
            case DOUBLE_TYPE: {
                printf("%f\n", result->d);
                break;
            }
            case STR_TYPE: {
                printf("%s\n", result->s);
                break;
            }
            case SYMBOL_TYPE:
                printf("%s\n", result->s);
                break;
            case CONS_TYPE:
               printf("I am printing from cons type\n");
               printTree(result);
                break;
            case NULL_TYPE:
                printf("null type in print result\n");
                break;
            case CLOSURE_TYPE:
                printf("I'm a closure type\n");
                printTree(result);
                break;
            default:
                printf("\n");
                //printf("got to bottom of print result\n");
        }
}


//primitive null implementation
Value *primitiveNull(Value *args) {
    Value *answer = talloc(sizeof(Value));
    answer->type = BOOL_TYPE;
    if (car(args)->type == CONS_TYPE) {
        if (length(car(args)) == 0) {
            answer->i = 1;
            return answer;
        }
    }
    if (length(args) == 0) {
        answer->i = 1;
    } else {
        answer->i = 0;
    }
    return answer;
}

//primitive star implementation
Value *primitiveStar(Value *args) {
    if (length(args) == 0 ) {
        printf("No arguments to multiply\n");
        evaluationError();
    }else if (length(args) < 2 ){
        printf("Not enough arguments to multiply\n");
        evaluationError();
    }
    Value *product = talloc(sizeof(Value));
    product->type = DOUBLE_TYPE;
    product->d = 1;
    while (args->type != NULL_TYPE) {
        switch(car(args)->type) {
            case INT_TYPE: {
                product->d = product->d * car(args)->i;
                break;
            }
            case DOUBLE_TYPE: {
                product->d = product->d * car(args)->d;
                break;

            }
            default:
                printf("type error in primitive star switch\n");
        }
        args = cdr(args);
    }
    return product;
}


//primitive minus implementation
Value *primitiveMinus(Value *args) {
    if (length(args) == 0 ) {
        printf("No arguments to subtract\n");
        evaluationError();
    }else if (length(args) < 2 ){
        printf("Not enough arguments to subtract\n");
        evaluationError();
    }
    Value *total = talloc(sizeof(Value));
    total->type = DOUBLE_TYPE;

    switch(car(args)->type) {
        case INT_TYPE: {
            total->d = car(args)->i; // *** need to cast to double type
            break;
        }
        case DOUBLE_TYPE: {
            total->d = car(args)->d;
            break;

        }
        default:
            printf("type error in primitive minus\n");
    }

    args = cdr(args);
    while (args->type != NULL_TYPE) {
        switch(car(args)->type) {
            case INT_TYPE: {
                total->d = total->d - car(args)->i;
                break;
            }
            case DOUBLE_TYPE: {
                total->d = total->d - car(args)->d;
                break;

            }
            default:
                printf("type error in primitive minus switch in while loop\n");
        }
        args = cdr(args);
    }
    return total;
}


Value *returnNumber(Value *args){
    Value *myInt= talloc(sizeof(Value));
    myInt->type = DOUBLE_TYPE;
    switch(car(args)->type) {
        case INT_TYPE:
            myInt->d = car(args)->i;
            break;
        case DOUBLE_TYPE:
            printf("in double type in return number\n");
            myInt->d = car(args)->d;
            break;
        default:
            printf("error in returnNumber\n");
    }
    return myInt;
}

//primitive <= implementation
Value *primitiveLessThanEqual(Value *args) {
    Value *myBoolean = talloc(sizeof(Value));
    myBoolean->type = BOOL_TYPE;
    if(length(args) == 2){
        if(returnNumber(args)->d <= returnNumber(cdr(args))->d){
            myBoolean->i = 1;
        }else{
            myBoolean->i = 0;
        }

    } else{
        evaluationError();
    }
    return myBoolean;
}


//primitive >= implementation
Value *primitiveGreaterThanEqual(Value *args) {
    Value *myBoolean = talloc(sizeof(Value));
    myBoolean->type = BOOL_TYPE;
    if(length(args) == 2){
        if(returnNumber(args)->d >= returnNumber(cdr(args))->d){
            myBoolean->i = 1;
        }else{
            myBoolean->i = 0;
        }

    } else{
        evaluationError();
    }
    return myBoolean;
}


//primitive division implementation
Value *primitiveDivision(Value *args) {
    if (length(args) == 0 ) {
        printf("No arguments to divide\n");
        evaluationError();
    }else if (length(args) < 2 ){
        printf("Not enough arguments to divide\n");
        evaluationError();
    }
    Value *answer = talloc(sizeof(Value));
    answer->type = DOUBLE_TYPE;
    switch(car(args)->type) {
        case INT_TYPE: {
            answer->d = car(args)->i;
            break;
        }
        case DOUBLE_TYPE: {
            answer->d = car(args)->d;
            break;

        }
        default:
            printf("type error in primitive division switch\n");
    }

    args = cdr(args);
    while (args->type != NULL_TYPE) {
        switch(car(args)->type) {
            case INT_TYPE: {
                answer->d = answer->d/car(args)->i;
                break;
            }
            case DOUBLE_TYPE: {
                answer->d = answer->d /car(args)->d;
                break;

            }
            default:
                printf("type error in primitive division switch in while loop\n");
        }
        args = cdr(args);
    }
    return answer;
}


//primitive modulo implementation
Value *primitiveModulo(Value *args) {
    if (length(args) == 0) {
        printf("No arguments to divide\n");
        evaluationError();
    } else if (length(args) < 2) {
        printf("Not enough arguments to divide\n");
        evaluationError();
    }
    int numOne;
    switch (car(args)->type) {
        case INT_TYPE: {
            numOne = car(args)->i;
            break;
        }
        case DOUBLE_TYPE: {
            numOne = (int)car(args)->d;
            break;

        }
        default:
            printf("type error in primitive modulo switch\n");
    }

    args = cdr(args);
    int numTwo;
    while (args->type != NULL_TYPE) {
        switch (car(args)->type) {
            case INT_TYPE: {
                numTwo = car(args)->i;
                break;
            }
            case DOUBLE_TYPE: {
                numTwo = (int)car(args)->d;
                break;

            }
            default:
                printf("type error in primitive modulo switch in while loop\n");
        }
        args = cdr(args);
    }
    Value *answer = talloc(sizeof(Value));

    answer->type = INT_TYPE;
    answer->i = 5 % 2;
    answer->i = numOne % numTwo;
    //printResult(numOne);
    //printResult(numTwo);
    return answer;
}



//primitive add implementation
Value *primitiveAdd(Value *args) {
    Value *sum = talloc(sizeof(Value));
    sum->type = DOUBLE_TYPE;
    sum->d = 0;
    if (primitiveNull(args)->i == 1) {
        printf("No arguments to add\n");
        evaluationError();
    } else if (length(args) < 1) {
        printf("Not enough arguments to add\n");
        evaluationError();
    } else {
        while (args->type != NULL_TYPE) {
            switch (car(args)->type) {
                case INT_TYPE: {
                    sum->d = sum->d + car(args)->i;
                    break;
                }
                case DOUBLE_TYPE: {
                    sum->d = sum->d + car(args)->d;
                    break;

                }
                default:
                    printf("type error in primitive add switch\n");

            }
            args = cdr(args);
        }
        return sum;
    }
}

//primitive car implementation
Value *primitiveCar(Value *args) {
    Value *carAnswer = talloc(sizeof(Value));
    if (primitiveNull(args)->i == 1) {
        printf("Impossible to get car of list\n");
        evaluationError();
    } else {
        if (car(car(car(args)))->type == CONS_TYPE) {
            Value *temp = makeNull();
            temp = cons(car(car(car(args))), temp);
            return temp;
        } else {
            carAnswer = car(car(car(args)));
        }
    }
    return carAnswer;
}


//primitive cdr implementation
Value *primitiveCdr(Value *args) {
    Value *temp = makeNull();
    Value *cdrAnswer = talloc(sizeof(Value));
    if (primitiveNull(args)->i == 1) {
        printf("Impossible to get cdr of list\n");
        evaluationError();
    } else {
        cdrAnswer = cdr(car(car(args)));
    }
    temp = cons(cdrAnswer, temp);
    return temp;
}


//primitive cons implementation
Value *primitiveCons(Value *args) {
    Value *head = makeNull();
    Value *dot = talloc(sizeof(Value));
    dot->type = SYMBOL_TYPE;
    dot->s  = ".";
    if(length(args) == 2) {
        if (car(args)->type != CONS_TYPE && car(cdr(args))->type != CONS_TYPE) {
            Value *temp = makeNull();
            temp = cons(car(cdr(args)), temp);
            temp = cons(dot, temp);
            temp = cons(car(args), temp);
            head = cons(temp, head);
        } else if (car(args)->type != CONS_TYPE && car(cdr(args))->type == CONS_TYPE) {
            Value *temp = cons(car(args), car(car(cdr(args))));
            head = cons(temp, head);
         //   return head;
        } else if (car(args)->type == CONS_TYPE && car(cdr(args))->type == CONS_TYPE) {
            Value *temp = cons(car(args), car(cdr(args)));
            head = cons(temp, head);
            return head;
        } else if (car(args)->type == CONS_TYPE && car(cdr(args))->type != CONS_TYPE) {
            head = cons(car(cdr(args)), head);
            head = cons(dot, head);
            head = cons(car(args), head);
            Value *tempList = makeNull();
            tempList = cons(head, tempList);
            return tempList;
        } else if (length(car(args)) == 0 || length(car(cdr(args))) == 0) {
            head = cons(args, head);
        }

    } else {
        printf("not enough arguments for cons\n");
        evaluationError();
    }
    Value *tempList = makeNull();
    tempList = cons(head, tempList);
    return tempList;
}


//binds primitive functions
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;
    Value *valueName = talloc(sizeof(Value));
    valueName->type = SYMBOL_TYPE;    //if this doesn't work, do STR_TYPE
    valueName->s = name;
    Value *consCell = cons(valueName, value);
    frame->bindings = cons(consCell, frame->bindings);
}


//a thin wrapper that calls eval for each top-level S-expression in the program.
// You should print out any necessary results before moving on to the next S-expression.
void interpret(Value *tree) {
    globalFrame = talloc(sizeof(Frame));
    globalFrame->bindings = makeNull();
    globalFrame->parent = NULL;
    bind("+", primitiveAdd, globalFrame);
    bind("null?", primitiveNull, globalFrame);
    bind("car", primitiveCar, globalFrame);
    bind("cdr", primitiveCdr, globalFrame);
    bind("cons", primitiveCons, globalFrame);

    bind("*", primitiveStar, globalFrame);
    bind("-", primitiveMinus, globalFrame);
    bind("<=", primitiveLessThanEqual, globalFrame);
    bind(">=", primitiveGreaterThanEqual, globalFrame);
    bind("/", primitiveDivision, globalFrame);
    bind("modulo", primitiveModulo, globalFrame);

    while (tree->type != NULL_TYPE) {
        Value *result;
        result = eval(car(tree), globalFrame);
        tree = cdr(tree);
        printResult(result);
    }
}


//Looks up the value that the symbol is bound to
Value *lookUpSymbol(Value *symbol, Frame *frame) {
    Frame *tempFrame = frame;
    Value *evaluated = talloc(sizeof(Value));

    while (tempFrame != NULL) { //go through all frames
        Value *temp = tempFrame->bindings;
        //printf("%s\n", tempFrame->bindings->s);
        //printResult(temp);
        //assert(tempFrame != NULL);
        if (tempFrame->bindings == NULL) {
            evaluationError();
        }
        while (temp->type != NULL_TYPE) {
            if(!strcmp(car(car(temp))->s, symbol->s)){
                evaluated = (cdr(car(temp)));
                return evaluated;
            } else { //***help
                temp = cdr(temp);
            }
        }
        tempFrame = tempFrame->parent;
    }
    evaluationError();
    return evaluated;
}

//evaluates each argument and returns what the last argument evaluates to
Value *evalBegin(Value *args, Frame *frame) {
    Value *argsHead = args;
    if (args->type == NULL_TYPE) {
        evaluationError();
    }
    while (cdr(argsHead)->type != NULL_TYPE) {
        eval(car(argsHead), frame);
        argsHead = cdr(argsHead);
    }
    return eval(car(argsHead), frame);
}


//helper function for eval let
Value *evalLetBindingsHelper(Value *args, Frame *frame){
    Value *bindings = makeNull();
    while(args->type != NULL_TYPE){
        Value *variable = (car(car(args)));
        Value *value = (car(cdr(car(args))));
        value = eval(value, frame);
        Value *newConsCell = cons(variable, value);
        bindings = cons(newConsCell, bindings);
        args = cdr(args);
    }
    return bindings;
}

//evaluates the let expression in different frames for each value
Value *evalLetStar(Value *args, Frame *oldFrame) {
    Value *argsHead = car(args);
    Value *tempArgs = args;
    Value *returnSomething = talloc(sizeof(Value));
    Value *currentBinding;
    Frame *newFrame = oldFrame;
    Value *oneBinding;
    while (argsHead->type != NULL_TYPE) {
        newFrame = talloc(sizeof(Frame));
        newFrame->parent = oldFrame;
        oneBinding = cons(car(argsHead), makeNull());
        currentBinding = evalLetBindingsHelper(oneBinding, oldFrame);
        newFrame->bindings = cons(car(currentBinding), makeNull());
        oldFrame = newFrame;
        argsHead = cdr(argsHead);
    }
    //move to last value within let
    while (cdr(cdr(tempArgs))->type != NULL_TYPE) {
        tempArgs = cdr(tempArgs);
    }
    //evaluate last value
    if (cdr(tempArgs)->type != NULL_TYPE) {
        return eval(car(cdr(tempArgs)), newFrame);
    }
    printf("Error: Invalid let syntax");
    texit(1);
    return returnSomething;
}


//Evaluates the let expression
Value *evalLet(Value *args, Frame *oldFrame) {
    Value *returnSomething = talloc(sizeof(Value));
    Value *bindings = evalLetBindingsHelper(car(args), oldFrame);
    Frame *nextFrame = talloc(sizeof(Frame));
    nextFrame->bindings = bindings;
    nextFrame->parent = oldFrame;
    //get last thing in body
    if (cdr(args)->type != NULL_TYPE) { //if there is a body
        while (cdr(args)->type != NULL_TYPE) { //get last thing in body
            args = cdr(args);
        }
        args = car(args);
        return eval(args, nextFrame); //evaluate let on last thing
    }else {
        printf("your let expression has no body\n");
        evaluationError();
    }
    evaluationError();
   return returnSomething;
}

//helper function for eval let
Value *evalLetBindingsHelperFalse(Value *args, Frame *frame){
    Value *bindings = makeNull();
    while(args->type != NULL_TYPE){
        Value *variable = (car(car(args)));
        Value *value = talloc(sizeof(Value));
        value->type = BOOL_TYPE;
        value->i = 0;
        Value *newConsCell = cons(variable, value);
        bindings = cons(newConsCell, bindings);
        args = cdr(args);
    }
    return bindings;
}


//Evaluates the let expression
Value *evalLetRec(Value *args, Frame *oldFrame) {
    Value *returnSomething = talloc(sizeof(Value));
    Value *argumentsValue = car(args);
    Frame *nextFrame = talloc(sizeof(Frame));
    nextFrame->parent = oldFrame;
    Value *bindings;
    //make default bindings pointing to false
    bindings = evalLetBindingsHelperFalse(car(args), nextFrame);
    Value *reversedBindings = reverse(bindings);
    nextFrame->bindings = reversedBindings;
    Value *bindingPointer = nextFrame->bindings;
    //change false to what the values evaluate to
    while (bindingPointer->type != NULL_TYPE) {
        Value *bindingVal = eval(car(cdr(car(argumentsValue))), nextFrame);
        bindingPointer->c.car->c.cdr = bindingVal;
        bindingPointer = cdr(bindingPointer);
        argumentsValue = cdr(argumentsValue);
    }
    //get last thing in body
    if (cdr(args)->type != NULL_TYPE) { //if there is a body
        while (cdr(args)->type != NULL_TYPE) { //get last thing in body
            args = cdr(args);
        }
        args = car(args);
        //printResult(lookUpSymbol(args, nextFrame));
        return eval(args, nextFrame); //evaluate let on last thing
    }
    else {
        printf("your let expression has no body\n");
        evaluationError();
    }
    evaluationError();
    return returnSomething;
}


//Evaluates the let expression
//Value *evalLetRec(Value *args, Frame *oldFrame) {
//    Value *returnSomething = talloc(sizeof(Value));
//    Value *arguments = car(args);
//    Value *parameter;
//    Frame *nextFrame = talloc(sizeof(Frame));
//    nextFrame->parent = oldFrame;
//    Value *binding;
//    Value *connectingCons;
//    while (arguments->type != NULL_TYPE) {
//        parameter = cons(car(arguments), makeNull());
//        binding = evalLetBindingsHelper(parameter, nextFrame);
//        //binding->c.cdr = nextFrame->bindings;
//        connectingCons = cons(car(binding),nextFrame->bindings);
//        nextFrame->bindings = connectingCons;
//        arguments = cdr(arguments);
//    }
//    //get last thing in body
//    if (cdr(args)->type != NULL_TYPE) { //if there is a body
//        while (cdr(args)->type != NULL_TYPE) { //get last thing in body
//            args = cdr(args);
//        }
//        args = car(args);
//        return eval(args, nextFrame); //evaluate let on last thing
//    }else {
//        printf("your let expression has no body\n");
//        evaluationError();
//    }
//    evaluationError();
//    return returnSomething;
//}


//checks the length of the args
bool argsLength(Value *args){
    int counter = 0;
    while (args->type != NULL_TYPE) {
        counter++;
        args = cdr(args);
    }
    if (counter == 3) {
        return true;
    } else {
        evaluationError();
        return false;
    }
}


//Evaluates the if expression
Value *evalIf(Value *args, Frame *frame) {
    bool lengthThree = argsLength(args);
    if (lengthThree != true) {
        evaluationError();
    } else {
        if (eval(car(args), frame)->i == true) {
            return (eval(car(cdr(args)), frame));
        } else {
            return (eval(car(cdr(cdr(args))), frame));
        }
    }
}


//Adds necessary bindings for define in the global frame
Value *evalDefine(Value *args, Frame *frame) {
    Value *voidType = talloc(sizeof(Value));
    voidType->type = VOID_TYPE;
    Value *variable = car(args);
    Value *expression = car(cdr(args));
    Value *evaluated = eval(expression, frame);
    Value *newBinding = cons(variable, evaluated);
    globalFrame->bindings = cons(newBinding, globalFrame->bindings);
    return voidType;
}


//Creates a closure containing function code, param names, and the frame
Value *evalLambda(Value *args, Frame *frame) {
    Value *newClosure = talloc(sizeof(Value));
    newClosure->type = CLOSURE_TYPE;
    newClosure->cl.frame = frame;
    newClosure->cl.paramNames = car(args);
    newClosure->cl.functionCode = car(cdr(args));
    return newClosure;
}

Value *evalSet(Value *args, Frame *frame) {
    Value *returnVal = talloc(sizeof(Value));
    returnVal->type = VOID_TYPE;
    Value *setTo = eval(car(cdr(args)), frame);
    Frame *tempFrame = frame;
    while (tempFrame != NULL) {
        Value *temp = tempFrame->bindings;
        while (temp->type != NULL_TYPE) {
            if(!strcmp(car(car(temp))->s, car(args)->s)){
               switch (setTo->type) {
                   case SYMBOL_TYPE:
                       cdr(car(temp))->s = setTo->s;
                       return returnVal;
                   case STR_TYPE:
                       cdr(car(temp))->s = setTo->s;
                       return returnVal;
                   case INT_TYPE:
                       cdr(car(temp))->i = setTo->i;
                       return returnVal;
                   case DOUBLE_TYPE:
                       cdr(car(temp))->d = setTo->d;
                       return returnVal;
                   case BOOL_TYPE:
                       cdr(car(temp))->i = setTo->i;
                       return returnVal;
                   case CONS_TYPE:
                       printf("cons type\n");
                       break;
                   case NULL_TYPE:
                       printf("null type\n");
                       break;
                   case CLOSURE_TYPE:
                       printf("closure type\n");
                       break;
                   default:
                       printf("there's a problem in evalSet\n");
                       return setTo;
               }
            } else {
                temp = cdr(temp);
            }
        }
        tempFrame = tempFrame->parent;
    }
    return returnVal;
}

//eval
Value *evalCond(Value *args, Frame *frame) {
    Value *returnCond = talloc(sizeof(Value));
    returnCond->type = VOID_TYPE;
    if (length(args) > 0) {
        while (args->type != NULL_TYPE) {
            Value *answer = eval(car(car(args)), frame);
            if (answer->i == true) {
                return car(cdr(car(args)));
            } else {
                args = cdr(args);
            }
        }
    }
    return returnCond;
}

Value *evalAnd(Value *args, Frame *frame) {
    Value *evalAndAnswer = talloc(sizeof(Value));
    if (length(args) == 0) {
        evalAndAnswer->type = BOOL_TYPE;
        evalAndAnswer->i = true;
    } else {
        while (args->type != NULL_TYPE) {
            evalAndAnswer = eval(car(args), frame);
            if (evalAndAnswer->i == false) {
                return evalAndAnswer;
            }
            args = cdr(args);
        }
    }
    return evalAndAnswer;
}

Value *evalOr(Value *args, Frame *frame) {
    Value *evalOrAnswer = talloc(sizeof(Value));
    if (length(args) == 0) {
        evalOrAnswer->type = BOOL_TYPE;
        evalOrAnswer->i = true;
    } else {
        while (args->type != NULL_TYPE) {
            evalOrAnswer = eval(car(args), frame);
            if (evalOrAnswer->i == true) {
                return evalOrAnswer;
            }
            args = cdr(args);
        }
    }
    return evalOrAnswer;
}

//Evaluates all params
Value *evalEach(Value *args, Frame *frame) {
    Value *head = makeNull();
    while (args->type != NULL_TYPE) {
        Value *evaled = eval(car(args), frame);
        head = cons(evaled, head);
        args = cdr(args);
    }
    Value *reversed = reverse(head);
    return reversed;
}


//maps the args to the params
Value *apply(Value *function, Value *args) {
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->bindings = makeNull();
    newFrame->parent = function->cl.frame;
    Value *argsTemp = args;
    if (function->type == PRIMITIVE_TYPE) {
        return function->pf(args);
    }
    Value *paramNamesTemp = function->cl.paramNames;
    while (argsTemp->type != NULL_TYPE && paramNamesTemp->type != NULL_TYPE) {
        Value *newConsCell = cons(car(paramNamesTemp), car(argsTemp));
        newFrame->bindings = cons(newConsCell, newFrame->bindings);
        paramNamesTemp = cdr(paramNamesTemp);
        argsTemp = cdr(argsTemp);
    }
    Value *result = eval(function->cl.functionCode, newFrame);
    return result;
}


//returns the value of the expression
Value *eval(Value *tree, Frame *frame) {
    Value *result = talloc(sizeof(Value));
    switch (tree->type) {
        case INT_TYPE: {
            return tree;
        }
        case BOOL_TYPE: {
            return tree;
        }
        case DOUBLE_TYPE: {
            return tree;
        }
        case STR_TYPE: {
            return tree;
        }
        case SYMBOL_TYPE: {
            return lookUpSymbol(tree, frame);
        }
        case NULL_TYPE:
            return tree;
        case CONS_TYPE: {
            Value *first = car(tree);
            Value *args = cdr(tree);
            if (first->type != SYMBOL_TYPE) {
                printf("First->type is not a symbol type in eval\n");
                evaluationError();
            }
            // Sanity and error checking on first...
            if (!strcmp(first->s, "if")) {
                return (evalIf(args, frame));
                //send it args
            } else if (!strcmp(first->s, "let")) {
                return evalLet(args, frame);

                // .. other special forms here
            }
            else if (!strcmp(first->s, "let*")) {
                return evalLetStar(args, frame);
            }
            else if (!strcmp(first->s, "letrec")) {
                return evalLetRec(args, frame);
            }

            else if (!strcmp(first->s, "begin")) {
                return evalBegin(args, frame);
            }
            else if (!strcmp(first->s, "quote")) {
                return args;
            } else if (!strcmp(first->s, "define")) {
                return evalDefine(args, frame);
            } else if(!strcmp(first->s, "lambda")) {
                return evalLambda(args, frame);
            } else if (!strcmp(first->s, "set!")) {
                return evalSet(args, frame);
            } else if (!strcmp(first->s, "cond")) {
                return evalCond(args, frame);
            } else if (!strcmp(first->s, "and")) {
                return evalAnd(args, frame);
            } else if (!strcmp(first->s, "or")) {
                return evalOr(args, frame);
            } else {
                Value *evaledOperator = eval(first, frame);
                Value *evaledArgs = evalEach(args, frame);
                return apply(evaledOperator,evaledArgs);
            }
        }
            default:
                printf("there's an error in eval\n");

        }
        printf("there's really an error in eval\n");
        evaluationError();
    return result;
}

