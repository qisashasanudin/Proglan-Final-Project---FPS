#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h> 

struct Node{ 
	int data;
	struct Node* next;
};

struct Node *current = NULL;

void insert(struct Node** head_ref, int new_data){
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
	new_node->data = new_data;
	new_node->next = NULL;
	
	if((*head_ref)==NULL) {
		(*head_ref) = new_node;
		return;
	}else{
		current = (*head_ref);
		while(current->next!=NULL){
			current = current->next;
		}
		current->next = new_node;	
	}
}

/*
void update_data(int old, int new) {
	int pos = 0;
	if(head==NULL) {
		printf("Linked List not initialized");
		return;
	}
	current = head;
	while(current->next!=NULL) {
		if(current->data == old) {
			current->data = new;
			printf("\n%d found at position %d, replaced with %d\n", old, pos, new);
			return;
		}
		current = current->next;
		pos++;
	}
	printf("%d does not exist in the list\n", old);
}
*/

void push(struct Node** head_ref, int new_data){ 
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
	new_node->data = new_data; 
	new_node->next = (*head_ref); 
	(*head_ref) = new_node; 
}

int GetNth(struct Node* head, int index){ 
	struct Node* current = head; 
	int count = 0;
	while (current != NULL){
		if (count == index){
			return(current->data); 
		}
		count++; 
		current = current->next;
	} 
	assert(0);			 
}

#endif
