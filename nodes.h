typedef struct node
{
    char data;
    struct node * next;
    struct node * prev;
} node;
typedef struct TDoubleLinkedList
{
    node *head;
    node *tail;
    int len;
} TDoubleLinkedList;
typedef struct ListaNode
{
    TDoubleLinkedList linie;
    struct ListaNode * urmator;
    struct ListaNode * anterior;
} ListaNode;
typedef struct ListaLista
{
    ListaNode *cap;
    ListaNode *coada;
    int nrlin;
} ListaLista;
typedef struct pozitie
{
    int x,y;
} pozitie;
typedef struct nodeSt
{
    char sir[100];
    struct nodeSt *next;
} nodeSt;
typedef struct stiva
{
    struct nodeSt *top;
    int len;
} stiva;
typedef struct nodeIndici
{
    char c;
    pozitie val;
    struct nodeIndici *next;
} nodeIndici;
typedef struct stivaIndici
{
    struct nodeIndici *top;
    int len;
} stivaIndici;
