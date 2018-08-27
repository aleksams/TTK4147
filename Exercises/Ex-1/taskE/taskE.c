#include <stdio.h>
#include "array.h"

void main(){
    Array kuk;
    long size = 69;
    kuk = array_new(size);
    for(int i = 0; i < 80; i++)
    {
    array_insertBack(&kuk, size);
    }
    array_print(kuk);
}
