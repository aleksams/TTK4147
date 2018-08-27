#include <stdio.h>
#include "array.h"

void main(){
    Array kuk;
    long size = 10;
    kuk = array_new(size);
    for(int i = 0; i < 10; i++){
        array_insertBack(&kuk, size);
    }
    array_reserve(&kuk, 20);
    array_print(kuk);
}
