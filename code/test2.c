#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char key;
    enum color
    {
        red=114,green=103,blue=98,
    } shirt;
    do
    {
        printf("please input r,g,b: ");
        scanf("%c",&key);
        fflush(stdin); //clear buffer
    } while((key!=red)&&(key!=green)&&(key!=blue)); //&&:AND

    shirt=key;

    switch(shirt)
    {
        case red:
            printf("choice red\n");
            break;
        case green:
            printf("choice green\n");
            break;
        case blue:
            printf("choice blue\n");
            break;
    }
    getchar();   
    return 0;
}