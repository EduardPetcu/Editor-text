#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "nodes.h"
#define LMAX 100
ListaLista *lis;
FILE *input,*output;
bool text_mode=1;
int middleInsert=0,cate;
pozitie cursor= {1,1},cc,semn= {0,0};
stiva *undo,*redo,*strings;
stivaIndici *ind,*indredo,*chars,*dw_pos;

void initS(stiva **s);
void add_stack (stiva *s, char *sir);
void add_stack_char(stivaIndici *s, char data);

void pop_stack(stiva *s);
void freeStackIND(stivaIndici **s);
void freeStack(stiva **s);

void freeList(ListaLista **list);
void freeTDList(TDoubleLinkedList list);

void initSI(stivaIndici **s);
void add_stack_indici (stivaIndici *s, pozitie data);
pozitie pop_stack_indici(stivaIndici *s);

void initL(ListaNode **list);
void initListaLista(ListaLista **lis);
void insertChar(ListaNode *list, char c,int p);
void insereaza(char line[]);
void print_line(ListaNode *list);
void print_ListaLista();
int read_command(char line[],int k);
void gl(char line[]);
void gc(char line[]);
void dl(char line[]);
void deleteLine(int n);
void deleteChar(pozitie cursor,int p);
void deleteMoreChars(int n,pozitie cursor,int p);
ListaNode* searchLine(int n);
void undoAction();
void redoAction();
void saveList();
void freee();
void da(char line[],int k);
void ra(char line[],int k);

int main()
{
    input=fopen("editor.in","r");
    initListaLista(&lis);
    initS(&undo);
    initS(&redo);
    initS(&strings);
    initSI(&ind);
    initSI(&indredo);
    initSI(&chars);
    initSI(&dw_pos);
    char line[LMAX];
    while(fgets(line,LMAX,input)!=NULL)
    {
        if(strstr(line,"::i")!=NULL)
        {
            text_mode=text_mode^1;
            if(middleInsert>1)
            {
                cursor.y=1;
                add_stack_indici(ind,cursor);
                insereaza(strings->top->sir);
                add_stack(undo,strings->top->sir);
                middleInsert=0;
            }

            middleInsert=0;
            add_stack(undo,line);
            continue;
        }
        if(text_mode)
        {
            add_stack_indici(ind,cursor);
            insereaza(line);
            add_stack(undo,line);
        }
        else
        {
            add_stack_indici(ind,cursor);
            if(read_command(line,0)!=5)
                add_stack(undo,line);
        }
    }
    return 0;
}
void initL(ListaNode **list)
{
    // initializarea unui nod de tip lista
    (*list) = malloc(sizeof(ListaNode));
    (*list)->anterior = NULL;
    (*list)->urmator = NULL;
    (*list)->linie.len=0;
    (*list)->linie.head=NULL;
    (*list)->linie.tail=NULL;
}
void initListaLista(ListaLista **lis)
{
    // initializarea unei liste de liste
    (*lis) = malloc(sizeof(ListaLista));
    (*lis)->cap = NULL;
    (*lis)->coada = NULL;
    (*lis)->nrlin=0;
}
void insertChar(ListaNode *list,char c,int p)
{
    // cand p=0 inseram inainte de caracterul 1
    // cand p=1 inseram dupa caracterul 1
    int contor=1;
    list->linie.len++;
    node *nou,*inainte,*inapoi;
    nou=(node*)malloc(sizeof(node));
    nou->data=c;
    nou->next=NULL;
    nou->prev=NULL;
    if(list->linie.head==NULL)
    {
        // daca lista pe care inseram este goala
        list->linie.head=nou;
        list->linie.tail=nou;
        return;
    }
    else if(cursor.y<=1 && p==0)
    {
        // inserare la inceputul liniei
        list->linie.head->prev=nou;
        nou->next=list->linie.head;
        list->linie.head=nou;
        return;
    }
    else if(cursor.y==list->linie.len || cursor.y==list->linie.len-1)
    {
        // inserare la sfarsitul listei
        list->linie.tail->next=nou;
        nou->prev=list->linie.tail;
        list->linie.tail=nou;
    }
    else
    {
        // inserare la mijloc
        inapoi=list->linie.head;
        inainte=list->linie.head->next;
        while(inapoi!=NULL && contor<cursor.y)
        {
            inapoi=inapoi->next;
            inainte=inainte->next;
            ++contor;
        }
        if(cursor.y==contor)
        {
            nou->next=inapoi->next;
            nou->prev=inapoi;
            inapoi->next=nou;
            inainte->prev=nou;
        }
    }
}
void insereaza(char line[])
{
    node *nou;
    char w[100];
    int n=strlen(line),i,contor=0;
    if(line[n-1]=='\n')
    {
        line[n-1]='\0';
        n--;
    }
    ListaNode *newLine,*inapoi;
    if(lis->nrlin==0)
    {
        //lista de liste e goala
        initL(&newLine);
        lis->cap=newLine;
        lis->coada=newLine;
        lis->nrlin++;
    }
    else if(cursor.x>lis->nrlin)
    {
        //adaugare linie la final
        initL(&newLine);
        lis->coada->urmator=newLine;
        newLine->anterior=lis->coada;
        lis->coada=newLine;
        lis->nrlin++;
    }
    else
    {
        // adaugare linie la mijloc
        inapoi=lis->cap;
        while(contor<cursor.x-1)
        {
            inapoi=inapoi->urmator;
            ++contor;
        }
        if(cursor.x-1==contor && middleInsert>0)
        {
            // se insereaza rand nou la mijloc doar daca se insereaza cel putin 2 randuri
            initL(&newLine);
            newLine->urmator=inapoi;
            newLine->anterior=inapoi->anterior;
            inapoi->anterior->urmator=newLine;
            inapoi->anterior=newLine;
            lis->nrlin++;
        }
    }
    if(cursor.y==1 && (cursor.x>=lis->nrlin || middleInsert>0))
    {
        // inserarea de caracter cu caracter pe randul nou inserat
        for(i=0; i<n; i++)
        {
            insertChar(newLine,line[i],0);
            cursor.y++;
        }
        cursor.x++;
        cursor.y=1;
        if(middleInsert>0)
        {
            // pentru cazul in care se insereaza cel putin 2 randuri
            // la mijlocul listei
            middleInsert++;
            deleteMoreChars(cate,cc,0);
            cate=0;
        }
    }
    else
    {

        for(i=n-1; i>=0; i--)
        {
             // se insereaza caracterele in ordine inversa
            // pe prima pozitie din linie
            insertChar(inapoi,line[i],(int)text_mode);
        }
        middleInsert++;
        cursor.y=cursor.y+n+1;
        nou=inapoi->linie.head;
        contor=1;
        while(contor<=cursor.y-1)
        {
            nou=nou->next;
            contor++;
        }
        n=inapoi->linie.len;
        for(i=cursor.y; i<=n; i++)
        {
            // pentru situatia in care se insereaza mai multe randuri la mijloc
            // in vectorul w se insereaza caracterele de dupa primul rand inserat la mijloc
            w[i-cursor.y]=nou->data;
            nou=nou->next;
        }
        w[n-cursor.y+1]='\0';
        add_stack(strings,w);
        cate=inapoi->linie.len-cursor.y+1;
        cc=cursor;
        cursor.x++;
        cursor.y=1;

    }
}
void print_line(ListaNode *list)
{
    node *nou2=list->linie.head;
    while(nou2!=NULL)
    {
        fprintf(output,"%c",nou2->data);
        nou2=nou2->next;
    }
    fprintf(output,"\n");
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
int read_command(char line[],int k)
{
    char com[3];
    com[0]=line[0];
    com[1]=line[1];
    com[2]='\0';
    if(!strcmp(com,"u\n") && k==0)
    {

        pop_stack_indici(ind);
        undoAction();
        return 5;
    }
    else if(!strcmp(com,"r\n") && k==0)
    {
        redoAction();
        return 5;
    }
    else if(!strcmp(com,"s\n"))
        saveList();
    else if(!strcmp(com,"q\n"))
        return 1;
    else if(!strcmp(com,"b\n"))
    {
        if(k)
            return 1;
        if(cursor.x>lis->nrlin)
        {
            cursor.x--;
            cursor.y=lis->coada->linie.len;
        } // Dupa inserare, cursorul meu se muta pe linia urmatoare.
        deleteMoreChars(1,cursor,1);
        cursor.y--;
    }
    else if(!strcmp(com,"dl"))
    {
        if(k)
            return 1;
        dl(line);
    }
    else if(!strcmp(com,"gl"))
    {
        if(k)
            return 1;
        gl(line);
    }
    else if(!strcmp(com,"gc"))
    {
        if(k)
            return 1;
        gc(line);
    }
    else if(!strcmp(com,"d ") || !strcmp(com,"d\n"))
    {
        if(k)
            return 1;
        if(line[2]>='0' && line[2]<='9')
            deleteMoreChars((int)line[2]-'0',cursor,0);
        else
            deleteMoreChars(1,cursor,0);
    }
    else if(!strcmp(com,"re"))
    {
        add_stack(undo,line);
        ra(line+3,1);
        return 5;
    }
    else if(!strcmp(com,"ra"))
    {
        add_stack(undo,line);
        ra(line+3,0);
        return 5;
    }
    else if(!strcmp(com,"dw"))
        da(line+3,1);
    else if(!strcmp(com,"da"))
        da(line+3,0);
    return 0;
}
void gl(char line[])
{
    cursor.x=(int)(line[3]-'0');
    cursor.y=0;
}
void gc(char line[])
{
    cursor.y=(int)(line[3]-'0');
    if(line[5]>='0' && line[5]<='9')
        cursor.x=(int)(line[5]-'0');
}
void dl(char line[])
{
    int n;
    if(line[3]>='0' && line[3]<='9')
        n=(int)(line[3]-'0');
    else
        n=cursor.x;
    deleteLine(n);
}
int min(int a,int b)
{
    if(a<b)
        return a;
    return b;
}
void deleteChar(pozitie cursor,int p)
{
    // p=0 stergem dupa cursor, p=1 stergem inainte de cursor
    int contor=1;
    ListaNode *precedent;
    node *nou,*nou2;
    if(cursor.x==1)
        precedent=lis->cap;
    else if(cursor.x>=lis->nrlin)
        precedent=lis->coada;
    else
    {
        precedent=lis->cap;
        while(precedent!=NULL && contor<cursor.x)
        {
            precedent=precedent->urmator;
            contor++;
        }
    }
    contor=1;
    nou=precedent->linie.head;
    if(precedent->linie.len==1)
    {
        //free(nou);
        precedent->linie.len--;
        deleteLine(cursor.x);
        return;
    }
    if(cursor.y<=1)
    {
        if(p==0 && cursor.y==1)
        {
            nou2=nou->next;
            add_stack_char(chars,nou2->data);
            if(precedent->linie.len>2)
                nou->next->next->prev=nou;
            nou->next=nou->next->next;
            free(nou2);
            precedent->linie.len--;
            if(precedent->linie.len==1)
                precedent->linie.tail->data=precedent->linie.head->data;
            return;
        }

        nou2=nou;
        add_stack_char(chars,nou2->data);
        nou=nou->next;
        nou->prev=NULL;
        precedent->linie.head=nou;
        precedent->linie.len--;
        if(precedent->linie.len==1)
            precedent->linie.tail->data=precedent->linie.head->data;
        free(nou2);
        return;
    }
    else if(cursor.y>=precedent->linie.len || (p==0 && cursor.y>=precedent->linie.len-1))
    {
        nou2=precedent->linie.tail;
        add_stack_char(chars,nou2->data);
        precedent->linie.tail->prev->next=NULL;
        precedent->linie.tail=precedent->linie.tail->prev;
        free(nou2);
    }
    else
    {
        while(nou!=NULL && contor<cursor.y-1)
        {
            nou=nou->next;
            contor++;
        }
        if(p==0)
            nou=nou->next;
        nou2=nou->next;
        add_stack_char(chars,nou2->data);
        nou->next->next->prev=nou;
        nou->next=nou->next->next;
        free(nou2);
    }
    precedent->linie.len--;
    if(precedent->linie.len==1)
        precedent->linie.tail->data=precedent->linie.head->data;
}
void deleteLine(int n)
{
    node *nou2;
    ListaNode *precedent=lis->cap,*nou;
    int contor=1,contor2=1;
    if(n==1)
    {
        nou=lis->cap;
        nou2=lis->cap->linie.head;
        do
        {
            contor2++;
            nou2=lis->cap->linie.head;
            lis->cap->linie.head=lis->cap->linie.head->next;
            free(nou2);
        }while(contor2<=lis->cap->linie.len);
        lis->cap->urmator->anterior=NULL;
        lis->cap=lis->cap->urmator;
        lis->nrlin--;
        free(nou);
    }
    else if(n>=lis->nrlin)
    {
        nou=lis->coada;
        nou2=lis->coada->linie.head;
        do
        {
            contor2++;
            nou2=lis->coada->linie.head;
            lis->coada->linie.head=lis->coada->linie.head->next;
            free(nou2);
        }while(contor2<=lis->coada->linie.len);
        lis->coada->anterior->urmator=NULL;
        lis->coada=lis->coada->anterior;
        lis->nrlin--;
        free(nou);
    }
    else
    {
        while(precedent!=NULL && contor<n-1)
        {
            precedent=precedent->urmator;
            contor++;
        }
        nou=precedent->urmator;
        nou2=precedent->urmator->linie.head;
        do
        {
            contor2++;
            nou2=precedent->urmator->linie.head;
            precedent->urmator->linie.head=precedent->urmator->linie.head->next;
            free(nou2);
        }while(contor2<=precedent->urmator->linie.len);
        precedent->urmator->urmator->anterior=precedent;
        precedent->urmator=precedent->urmator->urmator;
        lis->nrlin--;
        free(nou);
    }
}
void deleteMoreChars(int n,pozitie cursor,int p)
{
    int i;
    for(i=0; i<n; i++)
    {
        deleteChar(cursor,p);
        if(p)
            cursor.y--;
    }
}
void initS(stiva **s)
{
    (*s) = malloc(sizeof(stiva));
    (*s)->top=NULL;
    (*s)->len=0;
}
void add_stack(stiva *s, char sir[])
{
    nodeSt *p;
    p=(nodeSt*)malloc(sizeof(nodeSt));
    strcpy(p->sir,sir);
    s->len++;
    p->next=s->top;
    s->top=p;
}
void pop_stack(stiva *s)
{
    char valoare[LMAX];
    nodeSt *p;
    p=s->top;
    strcpy(valoare,p->sir);
    s->top=s->top->next;
    free(p);
    s->len--;
}
void initSI(stivaIndici **s)
{
    (*s) = malloc(sizeof(stivaIndici));
    (*s)->top=NULL;
    (*s)->len=0;
}
void add_stack_indici(stivaIndici *s, pozitie data)
{
    nodeIndici *nou;
    nou=(nodeIndici*)malloc(sizeof(nodeIndici));
    nou->val=data;
    s->len++;
    nou->next=s->top;
    s->top=nou;
}
void add_stack_char(stivaIndici *s, char data)
{
    nodeIndici *nou;

    nou=(nodeIndici*)malloc(sizeof(nodeIndici));
    nou->c=data;
    s->len++;
    nou->next=s->top;
    s->top=nou;
}
pozitie pop_stack_indici(stivaIndici *s)
{
    pozitie valoare;
    nodeIndici *p;
    p=s->top;
    valoare=p->val;
    s->top=s->top->next;
    free(p);
    s->len--;
    return valoare;
}
void undoAction()
{
    pozitie copieCursor,copieCursor2;
    int n,i,contor,nrranduri;
    ListaNode *inapoi;
    char p[LMAX];
    cursor=ind->top->val;

    strcpy(p,undo->top->sir);
    add_stack(redo,p);
    if(strstr(p,"::i")!=NULL)// undo de text
    {
        add_stack(redo,undo->top->sir);

        pop_stack(undo);

        strcpy(p,undo->top->sir);
        nrranduri=0;
        while(strstr(undo->top->sir,"::i")==NULL)
        {
            ++nrranduri;
            cursor=ind->top->val;
            n=strlen(p);
            if(cursor.y==1)
                cursor.y=0;
            deleteMoreChars(n,cursor,0);
            pop_stack_indici(ind);
            if(cursor.y==0)
                cursor.y=1;
            add_stack(redo,p);
            add_stack_indici(indredo,cursor);
            pop_stack(undo);
            strcpy(p,undo->top->sir);
        }

        if(cursor.x<lis->nrlin && nrranduri>1)
        {
            inapoi=lis->cap;
            contor=1;
            while(contor<cursor.x)
            {
                inapoi=inapoi->urmator;
                contor++;
            }
            strcpy(p,strings->top->sir);
            for(i=0; i<strlen(p); i++)
            {
                insertChar(inapoi,p[i],1);
                cursor.y++;
            }
            pop_stack(strings);
        }
        add_stack(redo,undo->top->sir);
    }
    else
    {
        add_stack_indici(indredo,cursor);
        if(strstr(p,"d ")!=NULL || strstr(p,"d\n")!=NULL)
        {
            {
                if(p[2]=='\0')
                    n=1;
                else
                    n=(int)(p[2]-'0');
                contor=1;
                inapoi=lis->cap;
                while(inapoi!=NULL && contor<cursor.x)
                {
                    inapoi=inapoi->urmator;
                    ++contor;
                }
            }
            for(i=1; i<=n; i++)
            {
                insertChar(inapoi,chars->top->c,0);
                pop_stack_indici(chars);
            }
        }
        else if(strstr(p,"ra ")!=NULL)
        {
            char *w1,*w2,line[LMAX];
            w2=strtok(p," \n");
            w2=strtok(NULL," \n");
            w1=w2;
            w2=strtok(NULL," \n");
            strcpy(line,w2);
            strcat(line," ");
            strcat(line,w1);
            ra(line,0);
        }
        else if(strstr(p,"re ")!=NULL)
        {
            char *w1,*w2,line[LMAX];
            w2=strtok(p," \n");
            w2=strtok(NULL," \n");
            w1=w2;
            w2=strtok(NULL," \n");
            strcpy(line,w2);
            strcat(line," ");
            strcat(line,w1);
            ra(line,1);
        }
        else if(strstr(p,"dw ")!=NULL || strstr(p,"da ")!=NULL)
        {
            char line[LMAX],*w1;
            w1=strtok(p," \n");
            w1=strtok(NULL," \n");
            strcpy(line,w1);
            copieCursor=cursor;
            n=strlen(line);
            while(dw_pos->top->val.x!=semn.x || dw_pos->top->val.y!=semn.y)
            {
                int contor=1;
                cursor=dw_pos->top->val;
                pop_stack_indici(dw_pos);
                ListaNode *precedent=lis->cap;
                if(cursor.x>=lis->nrlin)
                    precedent=lis->coada;
                else
                    while(precedent!=NULL && contor<cursor.x)
                    {
                        precedent=precedent->urmator;
                        contor++;
                    }
                insertChar(precedent,line[0],0);
                copieCursor2=cursor;

                for(i=1; i<n; i++)
                {
                    cursor.y++;
                    insertChar(precedent,line[i],1);
                }
                cursor=copieCursor2;
            }
            cursor=copieCursor;
        }
        else if(strstr(p,"gc")!=NULL || strstr(p,"gl")!=NULL)
        {
            cursor=ind->top->val;
        }
    }
    pop_stack(undo);
    pop_stack_indici(ind);
}
void redoAction()
{
    char p[LMAX];
    add_stack_indici(ind,cursor);//adaugam in stiva de undo
    cursor=indredo->top->val;
    strcpy(p,redo->top->sir);
    add_stack(undo,p);
    if(strstr(p,"::i")!=NULL)// undo de text
    {
        add_stack(undo,redo->top->sir);
        pop_stack(redo);
        strcpy(p,redo->top->sir);
        while(strstr(p,"::i")==NULL)
        {
            insereaza(p);
            add_stack(undo,p);
            pop_stack(redo);
            strcpy(p,redo->top->sir);
        }
        add_stack_indici(ind,cursor);
    }
    else
    {
        read_command(p,0);
        add_stack(undo,p);
    }
    pop_stack(redo);
    pop_stack_indici(indredo);
}
void freee()
{
    int contor=1,contor2;
    ListaNode *nou=lis->cap;
    node *nou2;
    while(contor<lis->nrlin)
    {
        nou=lis->cap;
        nou2=nou->linie.head;
        contor2=1;
        while(contor2<nou->linie.len)
        {
            nou2=nou->linie.head;
            nou->linie.head=nou->linie.head->next;
            free(nou2);
            contor2++;
        }
        //nou2=nou->linie.head;
        //free(nou2);
        free(nou);
        lis->cap=lis->cap->urmator;
        contor++;
    }
    nou=lis->cap;
    nou2=nou->linie.head;
    contor2=1;
    while(contor2<nou->linie.len)
    {
        nou2=nou->linie.head;
        nou->linie.head=nou->linie.head->next;
        free(nou2);
        contor2++;
    }
    free(nou2);
    free(nou);
    free(lis);
}
void freeStack(stiva **s)
{
    nodeSt *p=(*s)->top;
    int contor=1;
    while(contor<(*s)->len)
    {
        p=(*s)->top;
        (*s)->top=(*s)->top->next;
        free(p);
        contor++;
    }
    p=(*s)->top;
    free(p);
    free(*s);
}
void freeStackIND(stivaIndici **s)
{
    nodeIndici *p=(*s)->top;
    int contor=1;
    if((*s)->len<=0)
    {
        free(*s);
        return;
    }
    while(contor<(*s)->len)
    {
        p=(*s)->top;
        (*s)->top=(*s)->top->next;
        free(p);
        contor++;
    }
    p=(*s)->top;
    free(p);
    free(*s);
}
void saveList()
{
    output=fopen("editor.out","w");
    print_ListaLista();
    fclose(output);
}
void da(char line[],int k)
{
    int ok=0;
    if(cursor.y==0)
    {
        ok=1;
        cursor.y=1;
    }
    ListaNode *precedent=lis->cap;
    TDoubleLinkedList T;
    node *nou;
    int contor=1,n,contor2=1,i;
    n=strlen(line);
    if(line[n-1]=='\n')
    {
        line[n-1]='\0';
        --n;
    }
    add_stack_indici(dw_pos,semn);
    pozitie copieCursor=cursor,cc3;
    if(copieCursor.x>=lis->nrlin)
        precedent=lis->coada;
    else
        while(precedent!=NULL && contor<copieCursor.x)
        {
            precedent=precedent->urmator;
            contor++;
        }
    T=precedent->linie;
    nou=T.head;
    if(copieCursor.y>=T.len)
        nou=T.tail;
    else
        while(nou!=NULL && contor2<copieCursor.y)
        {
            nou=nou->next;
            contor2++;
        }
    while(copieCursor.x<=lis->nrlin)
    {

        while(copieCursor.y<=T.len-n+1)
        {
            if(line[0]==nou->data)
            {
                nou=nou->next;
                copieCursor.y++;
                for(i=1; i<n; i++)
                {
                    if(line[i]!=nou->data)
                        break;
                    nou=nou->next;
                    copieCursor.y++;
                }
                if(i==n)
                {
                    copieCursor.y--;
                    cc3=copieCursor;
                    cc3.y-=n;
                    add_stack_indici(dw_pos,cc3);
                    if(T.len==1)
                        deleteMoreChars(n,copieCursor,0);
                    else
                        deleteMoreChars(n,copieCursor,1);
                    if(k==1)
                    {
                        cursor.y-=ok;
                        return;
                    }
                    T.len=precedent->linie.len;
                    copieCursor.y=copieCursor.y-n+1;
                    if(T.len<=0)
                        copieCursor.x--;

                }
            }
            else
            {
                copieCursor.y++;
                nou=nou->next;
                continue;
            }
        }
        copieCursor.x++;
        copieCursor.y=1;
        if(precedent==lis->coada)
            break;
        precedent=precedent->urmator;
        T=precedent->linie;
        nou=T.head;
    }
    copieCursor.x=1;
    copieCursor.y=1;
    precedent=lis->cap;
    T=precedent->linie;
    nou=T.head;
    while(copieCursor.x<cursor.x)
    {
        while(copieCursor.y<=T.len-n+1)
        {
            if(line[0]==nou->data)
            {
                nou=nou->next;
                copieCursor.y++;
                for(i=1; i<n; i++)
                {
                    if(line[i]!=nou->data)
                        break;
                    nou=nou->next;
                    copieCursor.y++;
                }
                if(i==n)
                {
                    copieCursor.y--;
                    cc3=copieCursor;
                    cc3.y-=n;
                    add_stack_indici(dw_pos,cc3);
                    if(T.len==1)
                        deleteMoreChars(n,copieCursor,0);
                    else
                        deleteMoreChars(n,copieCursor,1);
                    if(k==1)
                    {
                        cursor.y-=ok;
                        return;
                    }
                    T.len=precedent->linie.len;
                    copieCursor.y=copieCursor.y-n+1;
                    if(T.len<=0)
                        copieCursor.x--;

                }
            }
            else
            {
                copieCursor.y++;
                nou=nou->next;
                continue;
            }
        }
        copieCursor.x++;
        copieCursor.y=1;
        precedent=precedent->urmator;
        T=precedent->linie;
        nou=T.head;
    }
    cursor.y+=ok;
}
void ra(char line[],int k)
{
    if(cursor.y==0)
        cursor.y=1;
    char *w1,*w2;
    w2=strtok(line," \n");
    w1=w2;
    w2=strtok(NULL," \n");
    ListaNode *precedent=lis->cap;
    TDoubleLinkedList T;
    node *nou,*nou2;
    int contor=1,n1,n2,contor2=1,i,j;
    n1=strlen(w1);
    n2=strlen(w2);
    pozitie copieCursor=cursor,copieCursor2,cc3;
    if(copieCursor.x>=lis->nrlin)
        precedent=lis->coada;
    else
        while(precedent!=NULL && contor<copieCursor.x)
        {
            precedent=precedent->urmator;
            contor++;
        }
    T=precedent->linie;
    nou=T.head;
    if(copieCursor.y>=T.len)
        nou=T.tail;
    else
        while(nou!=NULL && contor2<copieCursor.y)
        {
            nou=nou->next;
            contor2++;
        }
    while(copieCursor.x<=lis->nrlin)
    {
        while(copieCursor.y<=T.len-n1+1)
        {
            if(w1[0]==nou->data)
            {
                copieCursor2=copieCursor;//retinem pozitia de plecare
                copieCursor.y++;
                nou2=nou;
                nou=nou->next;
                for(i=1; i<n1; i++)
                {
                    if(w1[i]!=nou->data)
                        break;
                    nou=nou->next;
                    copieCursor.y++;//o sa ajunga pe pozitia copiecursor2.y+n1
                }
                if(i==n1)//am ajuns la finalul sirului
                {
                    for(j=1; j<=min(n1,n2); j++)
                    {
                        nou2->data=w2[j-1];
                        nou2=nou2->next;
                        copieCursor2.y++;
                    }
                    cc3=cursor;
                    if(n1>n2)
                    {
                        copieCursor.y--;
                        deleteMoreChars(n1-n2,copieCursor,1);
                        cursor=cc3;
                    }
                    else if(n1<n2)
                    {
                        cursor.x=copieCursor2.x;
                        cursor.y=copieCursor2.y-1;
                        for(; j<=n2; j++)
                            insertChar(precedent,w2[j-1],0);
                        cursor=cc3;
                        copieCursor.y=copieCursor.y+n2-n1;
                    }
                    if(k==1)
                        return;
                }
                T.len=T.len+n2-n1;
            }
            else
            {
                copieCursor.y++;
                nou=nou->next;
                continue;
            }
        }
        copieCursor.x++;
        copieCursor.y=1;
        if(precedent==lis->coada)
            break;
        precedent=precedent->urmator;
        T=precedent->linie;
        nou=T.head;
    }
    copieCursor.x=1;
    copieCursor.y=1;
    precedent=lis->cap;
    T=precedent->linie;
    nou=T.head;
    while(copieCursor.x<cursor.x)
    {
        while(copieCursor.y<=T.len-n1+1)
        {
            if(w1[0]==nou->data)
            {
                copieCursor2=copieCursor;//retinem pozitia de plecare
                copieCursor.y++;
                nou2=nou;
                nou=nou->next;
                for(i=1; i<n1; i++)
                {
                    if(w1[i]!=nou->data)
                        break;
                    nou=nou->next;
                    copieCursor.y++;//o sa ajunga pe pozitia copiecursor2.y+n1
                }
                if(i==n1)//am ajuns la finalul sirului
                {
                    for(j=1; j<=min(n1,n2); j++)
                    {
                        nou2->data=w2[j-1];
                        nou2=nou2->next;
                        copieCursor2.y++;
                    }
                    cc3=cursor;
                    if(n1>n2)
                    {
                        copieCursor.y--;
                        deleteMoreChars(n1-n2,copieCursor,1);
                        cursor=cc3;
                    }
                    else if(n1<n2)
                    {
                        cursor.x=copieCursor2.x;
                        cursor.y=copieCursor2.y-1;
                        insereaza(w2+j-1);
                        cursor=cc3;
                        copieCursor.y=copieCursor.y+n2-n1;
                    }
                    if(k==1)
                        return;
                }
                T.len=T.len+n2-n1;
            }
            else
            {
                copieCursor.y++;
                nou=nou->next;
                continue;
            }
        }
        copieCursor.x++;
        copieCursor.y=1;
        if(precedent==lis->coada)
            break;
        precedent=precedent->urmator;
        T=precedent->linie;
        nou=T.head;
    }
}
