#include"intercode.h"
#include <stdlib.h>

InterCode head=NULL;
InterCode tail=NULL;

void insertCode(InterCode c){
    c->prev=NULL;
    c->next=NULL;
    if(head==NULL){
        head=c;
        tail=c;
    }
    else{
        c->prev=tail;
        tail->next=c;
        tail=c;    
    }
}

void deleteCode(InterCode c){
    if(c==NULL||head==NULL||tail==NULL){
        return;
    }
    if(head==c&&tail==c){
        head=NULL;
        tail=NULL;
    }
    else if(head==c){
        head->next->prev=NULL;
        head=c->next;
    }
    else if(tail==c){
        tail->prev->next=NULL;
        tail=c->prev;
    }
    else{
        if(c->next!=NULL)
            c->next->prev=c->prev;
        if(c->prev!=NULL)
            c->prev->next=c->next;
    }
}











