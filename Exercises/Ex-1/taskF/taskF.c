#include <stdio.h>
#include "array.h"

void main(){
    Array a;
    long size = 10;
    a = array_new(size);
    for(int i = 0; i < 20; i++){
        array_insertBack(&a, size);
    }
    array_print(a);
}
