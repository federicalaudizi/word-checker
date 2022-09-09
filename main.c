#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int ric = 0;

typedef struct { // Define the Hash Table Item here
    char *key;
    int value;
} ht_item;

typedef struct LinkedList LinkedList;

struct LinkedList {
    ht_item *item;
    LinkedList *next;
};

typedef struct {
    // Contains an array of pointers to items
    ht_item **items;
    LinkedList **overflow_buckets;
    int size;
    int count;
} HashTable;

ht_item *ht_get(HashTable *table, char* key);

void free_table(HashTable *table);

void stampa_filtrate(HashTable* table);

void quicksort(char* arr[], int length);

void swap(char **a, char **b);

int
first_constraints(char *out, int i, ht_item *item, char *must_be, char *ref, int esc, HashTable *word_table, char *p,
                  HashTable **arr_constr, HashTable *hash_constr_table, HashTable* ref_table);

void
constraints(char *out, char *ref, char *p, HashTable *table, HashTable *new_table, HashTable *ref_table,
            HashTable *working_table, HashTable *hash_constr_table, char *must_be, HashTable **arr_constr);

void ht_delete(HashTable *table, int index, ht_item *item);

void new_round(HashTable *table, long int k);

HashTable *create_table(int size);

int get_index(char *str, int size);

unsigned long hash(char *str);

ht_item *create_item(char *key, int value, long int k);

void free_item(ht_item *item);

void ht_insert(HashTable *table, char *key, int value, long int k);

HashTable *resize(int size, HashTable *table, long int k);

void handle_collision(HashTable *table, unsigned long index, ht_item *item);

void print_table(HashTable *table);

HashTable *word_into_table(const char *str);

int ref_into_hash_index(int char_to_int_letter);

void compare(char *ref, char *p, long int k, HashTable *working_table, HashTable *table, HashTable *new_table,
             HashTable *ref_table, HashTable **arr_constrs, char *must_be, HashTable *hash_constr_table, char *out);

ht_item* ht_get(HashTable *table, char *key);

ht_item *ht_get_item(HashTable *table, char key);

int word_search(HashTable *table, char key);

void init_insert(HashTable *table, long int k, HashTable **arr_constr, char *must_be, HashTable *hash_constrs_table,
                 HashTable *admitted_table);

LinkedList *LinkedList_delete(LinkedList *head, char *key);

static LinkedList *allocate_list();

int hash_char_compare (HashTable* table, char c, int index);

void reset_working_table(HashTable  *ref_table, HashTable  *working_table);

int main() {
    //freopen("output.txt", "w+", stdout);
    //freopen("slide.txt", "r", stdin);
    long int k;
    char *ptr;
    char c[6];//I need it for fgets
    char* ret = fgets(c, 6, stdin);
    if (ret == NULL) {
        return 1;
    }
    c[strlen(c) - 1] = '\0';
    k = strtol(c, &ptr, 10);

    int var_len = (k + 1 > 20 ) ? (int)k+1 : 20;
    char input_str[var_len];

    int init_hash_size = 101;


    HashTable *table = create_table(init_hash_size);

    while (1) {
        ret = fgets(input_str, var_len+1, stdin);
        if (ret == NULL) {
            break;
        }
        input_str[strlen(input_str) - 1] = '\0'; // removing \n at the end

        // starting new "game"
        if (strcmp(input_str, "+nuova_partita") == 0) {
            // step 2.
            new_round(table, k);
            break;

        } else { // step 1. reading list of legal words
            // Inserting each string in my hash table
            if (table->count == round((init_hash_size * 0.7))) {
                init_hash_size = (init_hash_size * 2) + 1;
                table = resize(init_hash_size, table, k);
            }
            ht_insert(table, input_str, 0, k);
        }

    }
    free_table(table);
    return 0;
}

ht_item *create_item(char *key, int value, long int k) {  //ritorna il puntatore all'item che ho creato
    // Creates a pointer to a new hash table item
    ht_item *item = (ht_item *) malloc(sizeof(ht_item));
    item->key = (char *) malloc(1+k);
    strcpy(item->key, key);
    item->value = value;
    return item;
}

int get_index(char *str, int size) {
    return (int)(hash(str) % size);
}

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void ht_insert(HashTable *table, char *key, int value, long int k) {
    ht_item *item = ht_get(table, key);
    if(item != NULL) {
        item->value++;
        table->count++;
        return;
    }

    // key does not exist
    int index = get_index(key, table->size);
    item = create_item(key, value, k);
    if(table->items[index] == NULL) {
        table->items[index] = item;
        table->count++;
    } else {
        // append to bucket
        table->overflow_buckets[index] = linkedlist_insert(table->overflow_buckets[index], item);
        table->count++;
    }
}

HashTable *resize(int new_size, HashTable *old_table, long int k) {

    HashTable *new_table = create_table(new_size);

    for (int i = 0; i < old_table->size; i++) {
        if (old_table->items[i]) {
            ht_insert(new_table, old_table->items[i]->key, 0, k);
            if (old_table->overflow_buckets[i]) {
                LinkedList *head = old_table->overflow_buckets[i];
                while (head) {
                    ht_insert(new_table, head->item->key, 0, k);
                    head = head->next;
                }
            }
        }
    }
    free_table(old_table);
    return new_table;
}

static LinkedList *allocate_list() {
    // Allocates memory for a Linkedlist pointer
    LinkedList *list = (LinkedList *) malloc(sizeof(LinkedList));
    return list;
}


static LinkedList *linkedlist_insert(LinkedList *head, ht_item *item) {
    LinkedList *current = head;

    // create new node
    LinkedList *newNode = (LinkedList *) malloc(sizeof(LinkedList));
    if (newNode == NULL) {
        printf("malloc failed\n");
        exit(-1);
    }
    newNode->item = item;
    newNode->next = NULL;

    // if list is empty
    if(current == NULL) {
        head = newNode;
        return head;
    }

    while (current->next) {
        current = current->next;
    }
    // here current is last node, so append new node
    current->next = newNode;
    return head;
}


static void free_linkedlist(LinkedList *list) {
    LinkedList *temp = list;
    while (list) {
        temp = list;
        list = list->next;
        free_item(temp->item);
        free(temp);
    }
}

static LinkedList **create_overflow_buckets(HashTable *table) {
    // Create the overflow buckets; an array of linkedlists
    LinkedList **buckets = (LinkedList **) calloc(table->size, sizeof(LinkedList *));
    for (int i = 0; i < table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

static void free_overflow_buckets(HashTable *table) {
    // Free all the overflow bucket lists
    LinkedList **buckets = table->overflow_buckets;
    for (int i = 0; i < table->size; i++)
        free_linkedlist(buckets[i]);
    free(buckets);
}

//creates hash table
HashTable *create_table(int size) {
    // Creates a new HashTable
    HashTable *table = (HashTable *) malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;

    // dynamic array of pointers to items
    table->items = calloc(table->size, sizeof(ht_item *));
    for (int i = 0; i < table->size; i++) //sets all it’s items to NULL (Since they aren’t used)
        table->items[i] = NULL;
    //todo nella hashtable di ref non ci sono collisioni, ottimizza
    table->overflow_buckets = create_overflow_buckets(table);
    return table;
}

void free_item(ht_item *item) {
    // Frees an item
    free(item->key);
    free(item);
}

void free_table(HashTable *table) {
    // Frees the table
    for (int i = 0; i < table->size; i++) {
        if (table->items[i] != NULL){
            free_item(table->items[i]);
        }
    }
    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void handle_collision(HashTable *table, unsigned long index, ht_item *item) {
    LinkedList *head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    } else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}

void print_table(HashTable *table) {
    printf("\n-------------------\n");
    for (int i = 0; i < table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%s, Value:%d", i, table->items[i]->key, table->items[i]->value);
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                LinkedList *head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:%s, Value:%d ", head->item->key, head->item->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}

HashTable *word_into_table(const char *str) {
    int index, x;

    HashTable *table = create_table(64);
    for (int i = 0; i < strlen(str); i++) {
        x = str[i];
        index = ref_into_hash_index(x);

        char key[2] = "\0";
        key[0] = str[i];
        ht_item *item = create_item(key, 1, strlen(str));
        ht_item *current_item = table->items[index];
        if (current_item == NULL) {
            // Key does not exist.
            // Insert directly
            table->items[index] = item;
            table->count++;
        } else {
            // key exists: we only need to update value
            table->items[index]->value++;
        }
    }
    return table;
}

int ref_into_hash_index(int char_to_int_letter) {
    int index=0;
    //lettere maiuscole
    if (char_to_int_letter >= 65 && char_to_int_letter <= 90) {
        index = char_to_int_letter % 65;
    }
        //numeri
    else if (char_to_int_letter >= 48 && char_to_int_letter <= 57) {
        index = char_to_int_letter + 5;
    }
        //lettere minuscole
    else if (char_to_int_letter >= 97 && char_to_int_letter <= 122) {
        index = char_to_int_letter - 70;
    } else if (char_to_int_letter == 95) {
        index = 26;
    }
    if (char_to_int_letter == 45) {
        index = 63;
    }
    return index;
}


void compare(char *ref, char *p, long int k, HashTable *working_table, HashTable *table, HashTable *new_table,
             HashTable *ref_table, HashTable **arr_constrs, char *must_be, HashTable *hash_constr_table,
             char *out) {


    int count = 0;
    for (int i = 0; i < k; i++) {
        if (ref[i] == p[i]) {
            char pi[2];
            pi[0]=p[i];
            pi[1]='\0';
            ht_item *item = ht_get(working_table, pi);
            out[i] = '+';
            count++;
            item->value--;
        }
    }

    if (count == k) {
        out[0] = 'o';
        out[1] = 'k';
        out[2] = '\0';
        printf("ok\n");
        return;
    }

    //filling out "out"
    for (int i = 0; i < k; i++) {
        if (ref[i] != p[i]) {
            char pi[2];
            pi[0]=p[i];
            pi[1]='\0';
            ht_item *item = ht_get(working_table, pi);
            if (item != NULL) {
                if (item->value > 0) {
                    out[i] = '|';
                    item->value--;
                } else {
                    out[i] = '/';
                }
            } else {
                out[i] = '*';
            }
        }
    }

    //figuring out constraints
    constraints(out, ref, p, table, new_table, ref_table, working_table, hash_constr_table, must_be, arr_constrs);

    for (int i = 0; i < k; ++i) {
        if (out[i] == '*') {
            out[i] = '/';
        }
    }
    printf("%s\n", out);
    printf("%d\n", new_table->count);
}

ht_item *ht_get_item(HashTable *table, char key) {
    int x = key;
    int index = ref_into_hash_index(x);
    ht_item *item = table->items[index];

    // Ensure that we move to a non NULL item
    if (item != NULL) {
        if (item->key[0] == key)
            return item;
    }
    return NULL;
}

// returns item if exists else return NULL
ht_item *ht_get(HashTable *table, char* key) {
    int index = get_index(key, table->size);
    ht_item *item = table->items[index];

    // does not exist
    if(item == NULL) {
        return NULL;
    } else if(strcmp(item->key, key) == 0) {
        // exists in item (not in bucket)
        return item;
    } else {
        // check bucket
        LinkedList *head = table->overflow_buckets[index];
        while (head) {
            //printf("Key:%s, Value:%d ", head->item->key, head->item->value);
            if(strcmp(head->item->key, key) == 0) {
                return head->item;
            }
            head = head->next;
        }
        // not found in bucket
        return NULL;
    }
}


void init_insert(HashTable *table, long int k, HashTable **arr_constr, char *must_be, HashTable *hash_constrs_table,
                 HashTable *admitted_table) {

    int p_len = (k + 1 > 20 ) ? (int)k+1 : 20;
    char p[p_len];
    char *ret= fgets(p, p_len+1, stdin);
    if (ret == NULL) {
        return;
    }
    p[strlen(p) - 1] = '\0';
    HashTable *word_table;


    while (strcmp(p, "+inserisci_fine") != 0) {
        ht_insert(admitted_table, p, 0, k);
        word_table = word_into_table(p);
        for (int i = 0; i < k; i++) {
            if (must_be[i] != '+') {
                if (must_be[i] == p[i]) {

                    if (word_search(arr_constr[i], p[i]) != 0) {
                        int x = p[i];
                        int index = ref_into_hash_index(x);
                        if (hash_constrs_table->items[index] != NULL) {
                            if (hash_constrs_table->items[index]->value == word_table->items[index]->value) {
                                ht_insert(table, p, 0, k);
                            }
                        }
                    }
                } else{
                    break;
                }
            } else {

                if (word_search(arr_constr[i], p[i]) != 0) {
                    int x = p[i];
                    int index = ref_into_hash_index(x);
                    if (hash_constrs_table->items[index] != NULL) {
                        if (hash_constrs_table->items[index]->value == word_table->items[index]->value) {
                            ht_insert(table, p, 0, k);
                        }
                    }
                }
            }
        }
        ret = fgets(p, 1000, stdin);
        if (ret == NULL) {
            break;
        }
        p[strlen(p) - 1] = '\0';
    }
}

void new_round(HashTable *table, long int k) {
    int p_len = (k + 1 > 20 ) ? (int)k+1 : 20;
    char p[p_len], n_max[10];
    char ref[k+1];
    char must_be[k + 1];
    must_be[k] = '\0';
    char out[k + 1];
    out[k] = '\0';
    HashTable *hash_constr_table = create_table(64);
    HashTable *arr_constr[k];
    for (int i = 0; i < k; i++) {
        must_be[i] = '+';
        out[i] = '0';
        arr_constr[i] = create_table(64);
    }

    char* ret = fgets(ref, k + 2, stdin);
    if (ret == NULL) {
        return;
    }
    ref[k] = '\0';

    HashTable *ref_table = create_table(k);

    // iterate on string
    for (int j = 0; j < k; ++j) {
        char ref_i[2];
        ref_i[0] = ref[j];
        ref_i[1] = '\0';
        ht_insert(ref_table, ref_i, 1, k);
    }

    // printf("NEW REF TABLE\n");
    // print_table(ref_table);

    long int n;
    ret = fgets(n_max, 5, stdin);
    if (ret == NULL) {
        return;
    }
    n_max[(strlen(n_max) - 1)] = '\0';
    n=atoi(n_max);

    HashTable *new_table = create_table(table->size);

    int i = 0;

    HashTable *working_table = create_table(k);
    for (int g = 0; g < k; ++g) {
        char p_i[2];
        p_i[0] = ref[g];
        p_i[1] = '\0';
        ht_insert(working_table, p_i, 1, k);
    }

    while (i < n) {
        ret = fgets(p, p_len+1, stdin);

        if (ret == NULL) {
            break;
        }

        if (strncmp("+inserisci_inizio", p, 17) == 0) {
            if (new_table->count > 0) {
                init_insert(new_table, k, arr_constr, must_be, hash_constr_table, table);
            } else {
                init_insert(table, k, arr_constr, must_be, hash_constr_table, table);
            }
        } else if (strncmp(p, "+stampa_filtrate", 16) == 0) {
            if (new_table->count>0){
                stampa_filtrate(new_table);
            }else{
                stampa_filtrate(table); //todo sarebbe print table
            }
        } else {
            p[k] = '\0';

            if (ht_get(table, p)==NULL) {
                printf("not_exists\n");
            } else {
                compare(ref, p, k, working_table, table, new_table, ref_table, arr_constr, must_be,
                        hash_constr_table, out);

                if (strcmp(out, "ok") == 0) {
                    break;
                }
                reset_working_table(ref_table, working_table);
                i++;
            }
        }
    }


    if (i == n && strcmp(out, "ok") != 0) {
        printf("ko\n");
    }
    if (i == n || strcmp(out, "ok") == 0) {
        for (int j = 0; j < 2; j++) {
            ret = fgets(p, 1000, stdin);
            if (ret == NULL) {
                break;
            }
            p[strlen(p) - 1] = '\0';
            if (strncmp(p, "+inserisci_inizio", 17) == 0) {
                init_insert(table, k, arr_constr, must_be, hash_constr_table, table);
            } else if (strncmp(p, "+nuova_partita", 14) == 0) {
                ric = 0;
                j = 2;
                new_round(table, k);
            }
        }

        free_table(new_table);
        free_table(ref_table);
        free_table(working_table);

        for (int j = 0; j < k; ++j) {
            free_table(arr_constr[j]);
        }
        //free_table(hash_constr_table);
        return;
    }
}

void reset_working_table(HashTable  *ref_table, HashTable  *working_table) {

    for (int i = 0; i < working_table->size; i++) {
        if (working_table->items[i]) {
            //printf("Index:%d, Key:%s, Value:%d", i, working_table->items[i]->key, working_table->items[i]->value);
            working_table->items[i]->value = ref_table->items[i]->value;
            if (working_table->overflow_buckets[i]) {
                // printf(" => Overflow Bucket => ");
                LinkedList *w_head = working_table->overflow_buckets[i];
                LinkedList *r_head = ref_table->overflow_buckets[i];
                while (w_head) {
                    //printf("Key:%s, Value:%d ", head->item->key, head->item->value);
                    w_head->item->value = r_head->item->value;
                    w_head = w_head->next;
                    r_head = r_head->next;
                }
            }
        }
    }
}

void ht_delete(HashTable *table, int index, ht_item *item) {
    char *key = item->key;
    long int k = strlen(key);
    // Deletes an item from the table
    LinkedList *head = table->overflow_buckets[index];

    if (head == NULL) {
        // No collision chain. Remove the item
        // and set table index to NULL
        table->items[index] = NULL;
        free_item(item);
        table->count--;
        return;
    } else if (head != NULL) {
        // Collision Chain exists

        if (strcmp(item->key, table->items[index]->key) == 0) { //it the first item (not in the list
            free_item(item);
            table->count--;
            LinkedList *node = head;
            head = head->next;
            node->next = NULL;
            table->items[index] = create_item(node->item->key, node->item->value, k); //its somewhere in the list
            free_linkedlist(node);
            table->overflow_buckets[index] = head;

        } else {
            table->count--;
            table->overflow_buckets[index] = LinkedList_delete(head, item->key);
        }
    }
}


int word_search(HashTable *table, char key) { //se c'è ritorno 0
    int index = ref_into_hash_index((int) key);

    if (table->items[index] != NULL) {  // Ensure that we move to a non NULL item
        ht_item *item = table->items[index];
        if (item->key[0] == key)
            return 0;
    }
    return 1;
}

void constraints(char *out, char *ref, char *p, HashTable *table, HashTable *new_table, HashTable *ref_table,
                 HashTable *working_table, HashTable *hash_constr_table, char *must_be, HashTable **arr_constr) {
    unsigned long k = strlen(out);
    int i = 0, esc = 0, index;
    ht_item *item, *constr_items;
    LinkedList *head;
    HashTable *word_table;
    char ref_i[2] ;
    ref_i[1]='\0';

    for (int j = 0; j < table->size; j++) { //for (int j = 0; j < table->size; j++)
        // Ensure that we move to items which are not NULL
        if (ric == 0) {
            item = table->items[j];
            head = table->overflow_buckets[j];
        } else {
            item = new_table->items[j];
            head = new_table->overflow_buckets[j];
        }

        if (item != NULL) {
            if (head != NULL) {
                while (item != NULL) {
                    esc=0;
                    word_table = create_table(k); //create a table of each admitted word
                    for (int x = 0; x < k; ++x) {
                        char word_i[2];
                        word_i[0] = item->key[x];
                        word_i[1] = '\0';
                        ht_insert(word_table, word_i, 1, k);
                    }

                    while (i < k) {
                        ref_i[0] = ref[i];
                        ht_item *ref_item = ht_get(ref_table, ref_i);
                        if (ht_get(word_table, ref_item->key) != NULL) {
                            ht_item* temp_item = ht_get(working_table, ref_i);
                            if (temp_item->value == 0) { //numero esatto di caratteri
                                ht_item* temp_item1 = ht_get(ref_table, ref_i);
                                ht_item* temp_item2 = ht_get(word_table, ref_i);
                                if (temp_item1->value <= temp_item2->value) {
                                    index = ref_into_hash_index((int)ref[i]);
                                    if (hash_constr_table->items[index] ==NULL) { //hash_constr_table is the table where i save the number (min or exact) of each char
                                        ref_i[0] = ref[i];
                                        constr_items = create_item(ref_i, ref_item->value, k);
                                        hash_constr_table->items[index] = constr_items;
                                        hash_constr_table->count++;
                                    } else {
                                        if (hash_constr_table->items[index]->value < temp_item2->value) {
                                            hash_constr_table->items[index]->value = temp_item1->value;
                                        }
                                    }
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                            hash_constr_table, ref_table);
                                    i++;
                                    if (esc == 1) {
                                        break;
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else {
                                index = ref_into_hash_index((int)ref[i]);
                                ht_item* temp_item1 = ht_get(ref_table, ref_i);
                                ht_item* temp_item2 = ht_get(word_table, ref_i);
                                if (temp_item1->value == temp_item->value) {
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                            hash_constr_table);
                                    i++;
                                    if (esc == 1) {
                                        break;
                                    }
                                } else if ((temp_item1->value - temp_item->value) <=
                                           temp_item2->value) {
                                    if (hash_constr_table->items[index] == NULL) {
                                        ref_i[0] = ref[i];
                                        constr_items = create_item(ref_i, ref_item->value, k);
                                        hash_constr_table->items[index] = constr_items;
                                        hash_constr_table->count++;
                                    } else {
                                        if (hash_constr_table->items[index]->value < temp_item2->value) {
                                            hash_constr_table->items[index]->value = temp_item1->value;
                                        }
                                    }
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                            hash_constr_table);
                                    i++;
                                    if (esc == 1) {
                                        break;
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            }
                        } else {
                            ht_item* temp_item = ht_get(working_table, ref_i);
                            ht_item* temp_item1 = ht_get(ref_table, ref_i);
                            if (temp_item->value == temp_item1->value) {
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                        hash_constr_table);
                                i++;
                                if (esc == 1) {
                                    break;
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        }
                    }
                    i = 0;
                    if (ric == 0) {
                        if (esc == 0) {
                            ht_insert(new_table, item->key, 0, k);
                        }
                        if (head == NULL) {
                            break;
                        } else {
                            item = head->item;
                            head = head->next;
                        }
                    } else {
                        if (esc == 1) {
                            ht_delete(new_table, j, item);
                            if (head == NULL) {
                                break;
                            }
                            item = new_table->items[j];
                            head = new_table->overflow_buckets[j];
                        } else {
                            if (head == NULL) {
                                break;
                            } else {
                                item = head->item;
                                head = head->next;
                            }
                        }
                    }
                    free_table(word_table);
                }
            } else {
                word_table = create_table(k);
                for (int x = 0; x < k; ++x) {
                    char word_i[2];
                    word_i[0] = item->key[x];
                    word_i[1] = '\0';
                    ht_insert(word_table, word_i, 1, k);
                }
                while (i < k) {
                    ref_i[0] = ref[i];
                    ht_item *ref_item = ht_get(ref_table, ref_i);

                    if (ht_get(word_table, ref_item->key) != NULL) {
                        ht_item* temp_item = ht_get(working_table, ref_i);
                        if (temp_item->value == 0) {
                           //numero esatto di caratteri
                            ht_item* temp_item1 = ht_get(ref_table, ref_i);
                            ht_item* temp_item2 = ht_get(word_table, ref_i);
                            if (temp_item1->value <= temp_item2->value) {
                                index = ref_into_hash_index((int)ref[i]);
                                if (hash_constr_table->items[index] ==NULL) { //hash_constr_table is the table where i save the number (min or exact) of each char
                                    ref_i[0] = ref[i];
                                    constr_items = create_item(ref_i, ref_table->items[index]->value, k);
                                    hash_constr_table->items[index] = constr_items;
                                    hash_constr_table->count++;
                                } else {
                                    if (hash_constr_table->items[index]->value < temp_item2->value) {
                                        hash_constr_table->items[index]->value = temp_item1->value;
                                    }
                                }
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                        hash_constr_table);
                                i++;
                                if (esc == 1) {
                                    break;
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        } else {
                            ht_item* temp_item1 = ht_get(ref_table, ref_i);
                            ht_item* temp_item2 = ht_get(word_table, ref_i);
                            if (temp_item1->value == temp_item->value) {
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                        hash_constr_table);
                                i++;
                                if (esc == 1) {
                                    break;
                                }
                            } else if ((temp_item1->value - temp_item->value) <=
                                       temp_item2->value) {
                                index = ref_into_hash_index((int)ref[i]);
                                if (hash_constr_table->items[index] == NULL) {
                                    ref_i[0] = ref[i];
                                    constr_items = create_item(ref_i, ref_item->value, k);
                                    hash_constr_table->items[index] = constr_items;
                                    hash_constr_table->count++;
                                } else {
                                    if (hash_constr_table->items[index]->value < temp_item2->value) {
                                        hash_constr_table->items[index]->value = temp_item1->value;
                                    }
                                }
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                        hash_constr_table);
                                i++;
                                if (esc == 1) {
                                    break;
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        }
                    } else {
                        ht_item* temp_item = ht_get(working_table, ref_i);
                        ht_item* temp_item1 = ht_get(ref_table, ref_i);
                        if (temp_item->value == temp_item1->value) {
                            esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr,
                                                    hash_constr_table);
                            i++;
                            if (esc == 1) {
                                break;
                            }
                        } else {
                            esc = 1;
                            break;
                        }
                    }
                }
                i = 0;
                if (ric == 0) {
                    if (esc == 0) {
                        ht_insert(new_table, item->key, 0, k);
                    }
                } else {
                    if (esc == 1) {
                        ht_delete(new_table, j, item);
                    }
                }
                free_table(word_table);
            }
        }
    }
    ric = 1;
}

int first_constraints(char *out, int i, ht_item *item, char *must_be, char *ref, int esc, HashTable *word_table,
                      char *p, HashTable **arr_constr, HashTable* hash_constr_table, HashTable* ref_table) {
    int k = strlen(ref);

    if (out[i] == '+') {
        // letter that MUST appear at p[i]
        must_be[i] = ref[i];
        if (item->key[i] != ref[i]) {
            esc = 1;
        }

    } else if (out[i] == '*') {
        // letter that CANNOT appear anywhere in p
        // if p[i] not in parola ammissibile
        for (int l = 0; l < k; l++) {
            if (word_search(arr_constr[l], p[i]) != 0) {
                // create table
                ht_item *new_item = (ht_item *) malloc(sizeof(ht_item));
                new_item->key = (char *) malloc(1+k);
                new_item->key[0] = p[i];
                new_item->value = 1;
                int index = ref_into_hash_index((int) p[i]);
                arr_constr[l]->items[index] = new_item;
                arr_constr[l]->count++;
            }
        }
        char p_i[2];
        p_i[0] = p[i];
        p_i[1]='\0';
        if (ht_get(word_table, p_i) != NULL) {
            esc = 1;
        }
    } else if (out[i] == '|') {
        // letter that CANNOT appear at p[i]
        char p_i[2];
        p_i[0] = p[i];
        p_i[1]='\0';
        if (item->key[i] != p[i] && ht_get(word_table, p_i) != NULL) {
            if (arr_constr[i]->count == 0) {
                // inserisco in arr_constr a posizione i che quel carattere non ci può essere
                arr_constr[i] = word_into_table(&p[i]);
            } else if (word_search(arr_constr[i], p[i]) != 0) {
                // insert into existing table
                ht_item *new_item = (ht_item *) malloc(sizeof(ht_item));
                new_item->key = (char *) malloc(1+k);
                new_item->key[0] = p[i];
                new_item->value = 1;
                int index = ref_into_hash_index((int) p[i]);
                arr_constr[i]->items[index] = new_item;
                arr_constr[i]->count++;
            }
        } else {
            esc = 1;
        }
    } else {
        char p_i[2];
        p_i[0] = p[i];
        p_i[1]='\0';
        if (ht_get(word_table, p_i) != NULL) {
            if (arr_constr[i]->count == 0) {
                // inserisco in arr_constr a posizione i che quel carattere non ci può essere
                arr_constr[i] = word_into_table(&p[i]);
            } else if (ht_get(word_table, p_i) == NULL) {
                // insert into existing table
                ht_item *new_item = (ht_item *) malloc(sizeof(ht_item));
                new_item->key = (char *) malloc(1+k);
                new_item->key[0] = p[i];
                new_item->value = 1;
                int index = ref_into_hash_index((int) p[i]);
                arr_constr[i]->items[index] = new_item;
                arr_constr[i]->count++;
            }
            int index = ref_into_hash_index((int) p[i]);
            ht_item* temp_item1 = ht_get(ref_table, p_i);
            ht_item* temp_item2 = ht_get(word_table, p_i);
            if (temp_item2->value != temp_item1->value) {
                hash_constr_table->items[index] = temp_item1;
                esc = 1;
            }
            if (item->key[i]==p[i]){
                esc=1;
            }
        } else {
            esc = 1;
        }
    }

    return esc;
}

LinkedList *LinkedList_delete(LinkedList *head, char *key) {
    //temp is used to freeing the memory
    LinkedList *temp;

    //key found on the head node.
    //move to head node to the next and free the head.
    if (head->item->key == key) {
        temp = head;    //backup to free the memory
        head = head->next;
        free(temp);
    } else {
        LinkedList *current = head;
        while (current->next != NULL) {
            //if yes, we need to delete the current->next node
            if (current->next->item->key == key) {
                temp = current->next;
                //node will be disconnected from the linked list.
                current->next = current->next->next;
                free(temp);
                break;
            }
                //Otherwise, move the current node and proceed
            else
                current = current->next;
        }
    }
    return head;
}

/* Swaps position of strings in array (char**) */
void swap(char **a, char **b) {
    char *temp = *a;
    *a = *b;
    *b = temp;
}

void quicksort(char* arr[], int length) {
    unsigned int i, piv = 0;
    if (length <= 1)
        return;

    for (i = 0; i < length; i++) {
        // if curr str < pivot str, move curr into lower array and  lower++(pvt)
        if (strcmp(arr[i], arr[length -1]) < 0) 	//use string in last index as pivot
            swap(arr + i, arr + piv++);
    }


    //move pivot to "middle"
    swap(arr + piv, arr + length - 1);

    //recursively sort upper and lower
    quicksort(arr, piv++);			//set size to current pvt and increase for next call
    quicksort(arr + piv, length - piv);
}

void stampa_filtrate(HashTable* table){
    ht_item* item;
    LinkedList* head;
    char* filtered_words_array[table->count];
    int index=0;

    for (int i = 0; i < table->size; i++) {
        item = table->items[i];
        head = table->overflow_buckets[i];
        if (item != NULL) {
            if (head != NULL) {
                while (item != NULL) {
                    filtered_words_array[index]= item->key;
                    index++;

                    if (head == NULL) {
                        break;
                    }else{
                        item = head->item;
                        head = head->next;
                    }

                }
            }else{
                filtered_words_array[index]= item->key;
                index++;
            }
        }
    }
    quicksort(filtered_words_array, table->count);
    for (int i=0; i < table->count; i++) {
        printf("%s\n", filtered_words_array[i]);

    }
}

int hash_char_compare (HashTable* table, char c, int index){
    ht_item* item = table->items[index];
    LinkedList* head = table->overflow_buckets[index];
    if (item!=NULL){
        if (head==NULL){
            if (item->key[0] == c){
                return 0;
            }else{
                return 1;
            }
        }else{
            while (item!=NULL) {
                if (item->key[0] == c) {
                    return 0;
                } else {
                    item = head->item;
                    head = head->next;
                }
            }
        }
    }
    return 1;
}