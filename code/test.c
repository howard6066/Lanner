#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h> //引入字元測試與轉換函數標頭檔
#include <string.h>

typedef struct node
{
	int data;
	struct node *next;
} Node; 

int main(void)
{
  int val,num;
  Node *first,*current,*previous;
  printf("Number of nodes: ");
  scanf("%d",&num);
  
  for(int i=0;i<num;i++){
  	current=(Node *) malloc(sizeof(Node));//建立新節點
	printf("Data for node %d: ",i+1);
	scanf("%d",&(current->data)); //輸入節點的data成員
	if(i==0){
		first=current;  //如果是第一個成員把指標frist指向目前的節點 
	}else{
		previous->next=current;//把前一個的next指向目前的節點 
	}
	current->next=NULL; //把目前的節點的next指向NULL
	previous=current; //把前一個節點設成目前的節點 
  }
  
  current=first; //設current為第一個節點 
  while(current!=NULL){
  	printf("address=%p, ",current); //印出節點的位址 
  	printf("data=%d ",current->data); //印出節點的資料 
  	printf("next=%p\n",current->next); //印出下一個節點位址 
	current=current->next;  //將ptr指向下一個節點 
  } 

    getchar();   
    return 0;
}
