#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h> //引入字元測試與轉換函數標頭檔
#include <string.h>

void reverse(char str[]){
    int i=0,j=0;
    while(str[i]!='\0'){
        i++;
    }
    printf("reverse str: ");
    for(j=i-1;j>=0;j--){
        printf("%c",str[j]);
    }
    printf("\n");
}

int main(void)
{
    char str[50];
    printf("pls input str: ");
    fgets(str,50,stdin);
    reverse(str);

    getchar();   
    return 0;
}