#include <stdio.h>
#include <stdlib.h>

#define NAME "/testmassage"

int main(int argc, char**argv)
{
    int sum = 0;
    printf("%d\n", argc);
    for (size_t i = 0; i < argc; i++)
    {
        int tmp = atoi(argv[i]);
        printf("%d ", tmp);
    }
    
}