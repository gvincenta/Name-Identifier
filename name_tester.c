/* name identifier inside a sentence
 * Skeleton code written by Gilbert Vincenta, April 2018
 * Assignment done by Gilbert Vincenta, ID: 941088
 *
 * Algorithms are fun
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* typedefs and defines */
#define MAXNAME 30 /*maximum length for a name */
#define MAXDICT 100 /*maximum number of unique names allowed*/
#define MAXPROB 3 /*number of probabilites read*/
#define CH_HASH       '#'    /* hashtag */
#define MAXITEM (MAXDICT + 1) /*maximum number of items before a sentence,
including a line of % */

#define CH_ZERO '0' /* character zero*/
#define MAXPERCENTAGE 100 /* maximum percentage allowed */


#define FIRSTNAME 0 /* first name word probability at index 0 */
#define LASTNAME 1 /* last name word probability at index 1 */
#define NONE 2 /* non-name word probability at index 2 */

#define END_FLAG    '%'        /* indicator for end of dictionary */
#define OFFSET 3 /*print sentence beyond stage 3 */

/* typedefs and defines */

typedef int prob_t[MAXPROB];
typedef char name_t[MAXNAME+1];
typedef struct {
    char *name;
    prob_t prob;
    int name_len;
} dict_t;


/* typedefs and defines from Alistair Moffat, data type modified */

/* listops.c */
typedef struct node node_t;

struct node {
    name_t data;
    int data_len;
    node_t *next;
    prob_t result_stage_4;
    prob_t result_stage_5;
};

typedef struct {
    node_t *head;
    node_t *foot;
} list_t;

/* binarysearch.c */
#define BS_NOT_FOUND (-1)
#define BS_FOUND 0

/****************************************************************/

/* function prototypes */
/* stage 1 and 2 */
char *init_dict_name(void);
int process_name_char (dict_t *dict);
void process_name_prob (dict_t *dict);
void echo_stage_1_2(dict_t dict[], int dict_len);
void free_dict_name(dict_t dict[], int dict_len);

/* stage 3, 4, 5*/
list_t *read_sentence(list_t *sentence);
void echo_linked_list (list_t *sentence, int stage);
void determine_word(dict_t dict[], list_t *sentence, int dict_len);
int cmp(dict_t *dict, node_t *sentence);

/* function prototypes by Alistair Moffat */
/* getword.c */
int getword(char W[], int limit);
/* binarysearch.c */
int binary_search(dict_t dict[], int lo, int hi, node_t *key, int *locn);

/* listops.c */
list_t *insert_at_foot(list_t *list, char *value, int len);
void free_list(list_t *list);
list_t *make_empty_list(void);
/****************************************************************/

/* main function controls all the action */
int
main(int argc, char *argv[]) {
    dict_t dict[MAXDICT];  /* array of structure */
    int dict_len = 0, counter = 0 ;
    list_t *sentence;
    sentence = make_empty_list();
    /*runs stage 1 and 3*/
    for (counter = 0; counter < MAXITEM; counter++){
        /* reads in the dictionary's name */
        int end_flag = process_name_char (&dict[dict_len]);
        
        if (end_flag){
            /* no more item for dictionary, start reading sentence */
            sentence = read_sentence(sentence);
            break;
        }
        /* reads in the corresponding name's probability */
        process_name_prob(&dict[dict_len++]);
    }
    /* prints stage 1 and 2 */
    echo_stage_1_2(dict, dict_len);
    printf("\n");
    
    /* prints stage 3 */
    echo_linked_list (sentence, 3);
    printf("\n");
    
    /* runs stage 4 */
    determine_word(dict, sentence, dict_len);
    echo_linked_list (sentence, 4);
    printf("\n");
    
    /* runs stage 5 */
    echo_linked_list (sentence, 5);
    
    /* free up the linked list */
    free_list(sentence);
    sentence = NULL;
    free_dict_name(dict, dict_len);
    /* all done; take some rest */
    return 0;
}

void free_dict_name(dict_t dict[], int dict_len){
    int i;
    for (i = 0; i <= dict_len; i++){
        free(dict[i].name);
        dict[i].name = NULL;
    }
}

int process_name_char (dict_t *dict){
    char *temp_name;
    temp_name = init_dict_name();
    /* printf("counter mod = 0 \n"); */
    char ch;
    int end_flag = 0;
    int result_index = 0;
    
    /* appends the character into struct, but skip # */
    while (scanf("%c",&ch) == 1 && (ch != '\n')){
        if ( ch != CH_HASH && ch == END_FLAG){
            /* reads in the whole % until \n, but don't put it into dict[] */
            end_flag = 1;
        }
        
        else if ( ch != CH_HASH  && !end_flag){
            /* ignores # when appending into the array */
            temp_name[result_index++] = ch;
        }
    }
    temp_name[result_index] = '\0'; // length of word does not include null byte
    dict->name = temp_name;
    dict->name_len = result_index;
    return end_flag;
}

void process_name_prob (dict_t *dict){
    /*inserts into dict's probabilities */
    int first, last, none;
    scanf("%d%d%d\n", &first, &last, &none);
    assert(first+last+none == MAXPERCENTAGE);
    dict->prob[FIRSTNAME] = first;
    dict->prob[LASTNAME] = last;
    dict->prob[NONE] = none;
}

char *init_dict_name (void){
    /* initialises  array */
    char *name;
    name = (char *) malloc(sizeof(name_t));
    assert(name);
    return name;
}

void echo_stage_1_2(dict_t dict[], int dict_len ){
    /* prints out the very first word, total number of word,
     and average character */
    int word_count, char_count = 0;
    printf("=========================Stage 1=========================\n");
    for (word_count = 0; word_count < dict_len; word_count++){
        if (word_count == 0){
            printf("Name %d: %s\n",word_count, (dict[word_count].name));
            printf("Label probabilities: %d%% %d%% %d%%\n",
                   dict[word_count].prob[FIRSTNAME],
                   dict[word_count].prob[LASTNAME],
                   dict[word_count].prob[NONE]);
        }
        char_count += dict[word_count].name_len;
    }
    printf("\n");
    printf("=========================Stage 2=========================\n");
    printf("Number of names: %d\n", word_count);
    double result = (double) char_count / word_count;
    printf("Average number of characters per name: %.2f\n", result);
}

list_t *read_sentence(list_t *sentence){
    /* Queue implementation on reading sentence */
    name_t one_word;
    int one_word_len = 0;
    while ( (one_word_len = getword(one_word, MAXNAME)) != EOF){
        sentence = insert_at_foot(sentence, one_word, one_word_len);
    }
    return sentence;
}

void echo_linked_list (list_t *sentence, int stage){
    /*prints stage 3,4,5 */
    node_t *printer = sentence->head;
    char *value;
    printf("=========================Stage %d=========================\n",
           stage);
    while (printer){
        value = printer->data;
        if (stage - OFFSET){
            /*prints stage 4 and 5*/
            printf("%-32s", value);
            int i, first = 1;
            int *prob = printer->result_stage_4;
            if (stage == 5){
                prob = printer->result_stage_5;
            }
            for (i = 0; i < MAXPROB; i++){
                if (prob[i]) {
                    if (first){
                        first = 0;
                    }
                    else if (!first){
                        /* the word is identified to be multiple items */
                        printf(", ");
                    }
                    if (i == FIRSTNAME){
                        printf("FIRST");
                    }
                    else if (i == LASTNAME){
                        printf("LAST");
                    }
                    else if (i == NONE){
                        printf("NOT");
                    }
                    printf("_NAME");
                }
            }
            /*goes to next word in sentence */
            printf("\n");
        }
        else {
            /* prints stage 3 */
            printf("%s\n", value);
        }
        printer = printer->next;
    }
}

void determine_word(dict_t dict[], list_t *sentence, int dict_len){
    /* determines if the word is first/last/none name and store it */
    node_t *one_word = sentence->head;
    int locn;
    int first = 1;
    while(one_word){
        int *stage_5 = one_word->result_stage_5;
        int *stage_4=  one_word->result_stage_4;
        
        /* default probability = word not found for stage 4 and stage 5 */
        stage_5[FIRSTNAME] = stage_5[LASTNAME] = 0;
        stage_4[FIRSTNAME] = stage_4[LASTNAME] = 0;
        stage_5[NONE] = stage_4[NONE] = 1;
        
        if (binary_search(dict, 0, dict_len, one_word, &locn) == BS_FOUND) {
            /* turns on flag variable accordingly for stage 4 and 5*/
            
            int *prob = dict[locn].prob;
            
            /* determines for stage 4:
             if the word is found, it definitely is not none_name */
            if (prob[LASTNAME]){
                stage_4[LASTNAME] = 1;
                stage_4[NONE] = 0;
            }
            
            if (prob[FIRSTNAME]){
                stage_4[FIRSTNAME] = 1;
                stage_4[NONE] = 0;
            }
            
            /* determines for stage 5:
             it all depends on the probability of being a name
             against not a name */
            if (prob[FIRSTNAME] + prob[LASTNAME] > prob[NONE]){
                /* first detected name will be first name */
                if (first){
                    first = 0;
                    stage_5[FIRSTNAME] = 1;
                    stage_5[NONE] = 0;
                }
                else if (!first){
                    /* last name always comes adjacent to first name */
                    stage_5[LASTNAME] = 1;
                    stage_5[NONE] = 0;
                    first = 1;
                }
            }
            
            if (!first && (prob[FIRSTNAME] + prob[LASTNAME] < prob[NONE]) ){
                first = 1;
            }
        }
        one_word = one_word->next;
    }
}

int cmp(dict_t *dict, node_t *sentence) {
    /* runs strcnp for dict's name against a word in the sentence */
    char *dict_ptr = dict->name;
    char *sentence_ptr = sentence->data;
    return  strcmp (sentence_ptr, dict_ptr);
}

/****************************************************************/

/* getword.c by Alistair Moffat (modified) starts below */

/* Extract a single word out of the standard input, of not
 more than limit characters. Argument array W must be
 limit+1 characters or bigger. */
int getword(char W[], int limit) {
    int c, len = 0;
    /* first, skip over any non alphabetics */
    while ((c=getchar())!=EOF && !isalpha(c)) {
        /* do nothing more */
    }
    if (c==EOF) {
        return EOF;
    }
    /* ok, first character of next word has been found */
    W[len++] = c;
    while (len<limit && (c=getchar())!=EOF && isalpha(c)) {
        /* another character to be stored */
        W[len++] = c;
    }
    /* now close off the string */
    W[len] = '\0';
    return len;
}

/* binarysearch.c by Alistair Moffat (modified) starts below */
int binary_search(dict_t dict[], int lo, int hi, node_t *key, int *locn) {
    
    int mid, outcome;
    /* if key is in dict, it is between dict[lo] and dict[hi-1] */
    if (lo>=hi) {
        return BS_NOT_FOUND;
    }
    
    mid = (lo+hi)/2;
    if ((outcome = cmp(&dict[mid], key)) < 0) {
        return binary_search(dict, lo, mid, key, locn);
    }
    else if (outcome > 0) {
        return binary_search(dict, mid+1, hi, key, locn);
    }
    else {
        *locn = mid;
        return BS_FOUND;
    }
}

/* listops.c by Alistair Moffat starts below
 * *insert_at_foot(list_t *list, data_t value) is modified */
list_t *make_empty_list(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    return list;
}

void free_list(list_t *list) {
    node_t *curr, *prev;
    assert(list!=NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}

list_t *insert_at_foot(list_t *list, char *value, int len) {
    
    node_t *new;
    
    /* initialises new node */
    new = (node_t*)malloc(sizeof(*new));
    assert(list!=NULL && new!=NULL);
    
    /* copies value into the new node */
    strcpy(new->data,value);
    new->data_len = len;
    new->next = NULL;
    
    if (list->foot==NULL) {
        /* this is the first insertion into the list */
        list->head = list->foot = new;
    } else {
        /* appends into linked list */
        list->foot->next = new;
        list->foot = new;
    }
    return list;
}

/* =====================================================================
 (Some) programmes written by Alistair Moffat, as an example for the book
 "Programming, Problem Solving, and Abstraction with C", Pearson
 Custom Books, Sydney, Australia, 2002; revised edition 2012,
 ISBN 9781486010974.
 
 See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
 information.
 
 Prepared December 2012 for the Revised Edition.
 ================================================================== */
/****************************************************************/
