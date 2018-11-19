#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
 
int main()
{
    system("chcp 1251");
    setlocale(LC_ALL, "UTF8");

    char str[80];

    printf("Въведи текст\n");
    printf("Привет мир\n");
    scanf("%s", str);

    printf("Ти въведе: %s", str);

    getchar();

    return 0;
}

