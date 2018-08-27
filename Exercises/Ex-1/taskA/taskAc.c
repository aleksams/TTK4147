#include <stdio.h>
#include <sys/times.h>


void busy_wait(clock_t t);

void main()
{
    struct tms buf;
    clock_t t;
    t = 100;
    busy_wait(t);
}

void busy_wait( clock_t t){
    struct tms buf;
    clock_t now;

    now = times(&buf);

    clock_t then = now + t;

    while(((double) now - then) < 0){
        for(int i = 0; i < 10000; i++){}
        now = times(&buf);
    }
}
