#include "nodes.h"
void initL(TDoubleLinkedList **list)
{
    (*list) = malloc(sizeof(TDoubleLinkedList));
    (*list)->head = NULL;
    (*list)->tail = NULL;
    (*list)->len=0;
}
void initListaLista()
{
    lis = malloc(sizeof(ListaLista));
    lis->cap = NULL;
    lis->coada = NULL;
    lis->nrlin=0;
}
void insertChar(TDoubleLinkedList *list, char c)
{
    list->len++;
    node *nou;
    nou=(node*)malloc(sizeof(node));
    nou->data=c;
    nou->next=NULL;
    nou->prev=NULL;
    if(list->head==NULL)
    {
        list->head=nou;
        list->tail=nou;
        return;
    }
    list->tail->next=nou;
    nou->prev=list->tail;
    list->tail=nou;
}
void insereaza(char line[])
{
    int n=strlen(line),i;
    TDoubleLinkedList *sir;
    initL(&sir);
    ListaNode *newLine;
    newLine=(ListaNode*)malloc(sizeof(ListaNode));
    if(lis->nrlin==0)//lista de linii este goala
    {
        lis->cap=newLine;
        lis->coada=newLine;
        lis->nrlin++;
    }
    else
    {
        lis->coada->urmator=newLine;
        newLine->anterior=lis->coada;
        lis->coada=newLine;
        lis->nrlin++;
    }
    for(i=0;i<n;i++)
    {
        insertChar(sir,line[i]);
    }
    (newLine->linie)=*sir;
}
void print_line(TDoubleLinkedList *list)
{
    node *nou2=list->head;
    while(nou2!=NULL)
    {
        fprintf(output,"%c",nou2->data);
        nou2=nou2->next;
    }
}
void print_ListaLista()
{
    int contor=0;
    ListaNode *nou=lis->cap;
    while(contor<lis->nrlin)
    {
        print_line(nou);
        nou=nou->urmator;
        contor++;
    }
}
