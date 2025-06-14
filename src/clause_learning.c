#include <stdio.h>
#include <stdlib.h>
#include "dpll.h"
#include "formula.h"
# define swap(a,b) {node t; t = a; a = b; b = t;}
# define max(a,b) ((a > b) ?  a : b )

typedef struct node node;
struct node
{
    double countPos;
    double countNeg;
    int variable;
    double score; // max(countPos,countNeg)
};

node *vector;

double jeroslow(int c)
{
    return 2/c;
}

void insertionSort(node *vetor, int l, int r)
{
    int aux,j;
    int i = l + 1;
    while(i <= r)
    {
        aux = vetor[i].score;
        j = i;
        while(j  > 0 && aux < vetor[j - 1].score)
        {
            vetor[j] = vetor[j - 1];
            j--;
        }

        vetor[j].score = aux;
        i++;
    }
}

int partition( node *v , int l , int r )
{
    int i = l;
    int j = l ;
    int pivo = v[r].score;

    while (i < r)
    {
        if(v[i].score < pivo)
        {   
            if(i != j)
                swap(v[i], v[j]);
            j++;
        }
        i++;
    }
    
    swap (v[j] ,v[r]);

    return j;
}

void quickSort(node v[], int l, int r)
{
    if(l < r)
    {   
        // coloca a mediana na ultima posicao, mediana de 3
        int meio = (l + r)/2; 

        if(v[meio].score > v[r].score) swap(v[meio], v[r]);
        if(v[l].score > v[meio].score) swap(v[l], v[meio]);
        if(v[r].score > v[meio].score) swap(v[r],v[meio]);

        if(r - l + 1 <= 15 )
            insertionSort(v, l, r);
        else
        {
            int p = partition(v, l, r);
            quickSort(v, l, p-1);
            quickSort(v, p+1, r);
        }
    }
}

void PreProcessing(Form* form){
    ClauseNode* head = form->clauses;
    vector = malloc((form->numVars + 1) * sizeof(node));
    int literal;
    Clause *clause;
    while(head != NULL)
    {
        clause = head->clause;

        for(int j = 0; j < clause->size; ++j)
        {
            //printf("%d ", pivot->clause->literals[j]);
            literal = clause -> literals[j];
            if (literal < 0)
                vector[abs(literal)].countNeg  += jeroslow(clause -> size);
            else
                 vector[literal].countNeg  += jeroslow(clause -> size);
        }

        head = head->next;
        //printf("\n");
    }
    head = form->clauses;

    for(int i = 1; i <= form->numVars + 1)
        vector[i].score = max(vector[i].countNeg, vector[i].countPos);
    quickSort(vector, 1, form -> numVars);
}

enum DecideState Decide(const Form* form)
{

    ClauseNode* list = form->clauses;
    while(list != NULL)
    {

        Clause *clause = list->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];
            lit = ((lit > 0 )? lit : -lit);
            if(getVarState(lit-1) == UNK)
            {
                insertDecisionLevel(lit-1, FALSE);
                
                return FOUND_VAR;
            }
        }

        list = list->next;
    }

    return ALL_ASSIGNED;
}

bool BCP(Form *formula, const Decision decision)
{
    bool flag;
    ClauseNode *head;
    Clause *clause;

    LiteralId falseValuedLiteral = ((decision.value == FALSE) ? decision.id+1 : -decision.id -1);

    head = formula->literals[getPos(falseValuedLiteral)];



    // now if some clause have only negative
    // values than this is a conflict
    while(head!=NULL)
    {
        flag = false;
        clause = head->clause;

        for(int i = 0; i<clause->size; ++i)
        {
            LiteralId lit = clause->literals[i];

            if(getLitState(lit) != FALSE)
                flag=true;
        }

        if(!flag){
            return false;
        }

        head = head->next;
   }

   return true;
}

int resolveConflict()
{
    Decision* decisions =  getDecisions();

    int i = getLevel()-1;

    for(; i>=0; --i)
        if(decisions[i].flipped == false)
            break;

    return i+1;
}

