#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void first_constraints(char *out, const char *ref, const char *p, HashTable *table, HashTable *new_table,
                       HashTable *ref_table, HashTable *working_table);

void ht_delete(HashTable *table, int index, ht_item *item);

void new_round(HashTable *table, long int k);

HashTable *create_table(int size);

int get_index(unsigned char *str, int size);

unsigned long hash(unsigned char *str);

ht_item *create_item(char *key, int value, long int k);

void ht_insert(HashTable *table, char *key, int value, long int k);

void resize(int size, HashTable *table, long int k);

void handle_collision(HashTable *table, unsigned long index, ht_item *item);

void print_table(HashTable *table);

HashTable *ref_into_hash(const char *str, long int k);

int ref_into_hash_index(int char_to_int_letter);

HashTable *
compare(const char *ref, const char *p, long int k, HashTable *working_table, HashTable *table, HashTable *new_table,
        HashTable *ref_table);

int ht_admitted_search(HashTable *table, char *key);

ht_item *ht_get_item(HashTable *table, char key);

int word_search(HashTable *table, char key);

void init_insert(HashTable *table, long int k);

int main() {
    //freopen("output.txt", "w+", stdout);
    long int k;
    char *ptr;
    char c[2];//I need it for fgets
    fgets(c, 4, stdin);
    c[strlen(c) - 1] = '\0';
    k = strtol(c, &ptr, 10);
    char input_str[k];
    int init_hash_size = 53;

    HashTable *table = create_table(init_hash_size);
    table->count = 0;

    while (1) {
        fgets(input_str, 1000, stdin);
        input_str[strlen(input_str) - 1] = '\0'; // removing \n at the end

        // starting new "game"

        if (strcmp(input_str, "+nuova_partita") == 0) {
            // step 2.
            new_round(table, k);
            break;

        } else { // step 1. reading list of legal words
            // Inserting each string in my hash table
            ht_insert(table, input_str, 0, k);
        }

    }
    return 0;
}

ht_item *create_item(char *key, int value, long int k) {  //ritorna il puntatore all'item che ho creato
    // Creates a pointer to a new hash table item
    ht_item *item = (ht_item *) malloc(sizeof(ht_item));
    item->key = (char *) malloc(k);
    strcpy(item->key, key);
    item->value = value;
    return item;
}

int get_index(unsigned char *str, int size) {
    return hash(str) % size;
}

unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void ht_insert(HashTable *table, char *key, int value, long int k) {
    ht_item *item = create_item(key, value, k);        //create an item
    int index = get_index(key, table->size);          //apply hash function

    ht_item *current_item = table->items[index];
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");               /*devo capire quando metto piu di 50 parole ammissibili mannaggia*/
            return;
        }
        // Insert directly
        table->items[index] = item;
        table->count++;
    } else {
        // Scenario 1: We only need to update value
        if (strcmp(current_item->key, key) == 0) {
            table->items[index]->value = value;
            return;
        } else {
            // Scenario 2: Collision
            table->count++;
            handle_collision(table, index, item);
            return;
        }
    }
}

void resize(int size, HashTable *table, long int k) {

    int oldTableSize = size;
    HashTable *new_table;

    size = oldTableSize * 2;
    new_table = create_table(size);

    for (int i = 0; i < size; i++)
        new_table->items[i] = NULL;

    size = 0;

    for (int hash = 0; hash < oldTableSize; hash++)
        if (table->items[hash] != NULL) {
            ht_insert(new_table, table->items[hash]->key, table->items[hash]->value, k);
        }

    for (int i = 0; i <= oldTableSize; ++i) {
        free(table->items[i]);
    }
}

static LinkedList *allocate_list() {
    // Allocates memory for a Linkedlist pointer
    LinkedList *list = (LinkedList *) malloc(sizeof(LinkedList));
    return list;
}

static LinkedList *linkedlist_insert(LinkedList *list, ht_item *item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList *head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } else if (list->next == NULL) {
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList *temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }

    LinkedList *node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;

    return list;
}

static ht_item *linkedlist_remove(LinkedList *list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList *node = list->next;
    LinkedList *temp = list;
    temp->next = NULL;
    list = node;
    ht_item *it = NULL;
    memcpy(temp->item, it, sizeof(ht_item));
    free(temp->item->key);
    free(temp->item);
    free(temp);
    return it;
}

static void free_linkedlist(LinkedList *list) {
    LinkedList *temp = list;
    while (list) {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item);
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
    table->overflow_buckets = create_overflow_buckets(
            table); //todo nella hashtable di ref non ci sono collisioni, ottimizza
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
        ht_item *item = table->items[i];
        if (item != NULL)
            free_item(item);
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

HashTable *ref_into_hash(const char *str, long int k) {
    int index, x;

    HashTable *table = create_table(64);
    for (int i = 0; i < k; i++) {
        x = str[i];
        index = ref_into_hash_index(x);

        char key[2] = "\0";
        key[0] = str[i];
        ht_item *item = create_item(key, 1, k);
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
    int index;
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

int ht_admitted_search(HashTable *table, char *key) {
    // Searches the key in the hashtable and returns 1 if it exists, 0 otherwise
    int index = get_index(key, table->size);
    ht_item *item = table->items[index];
    LinkedList *head = table->overflow_buckets[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return 1;
        if (head == NULL)
            return 0;
        item = head->item;
        head = head->next;
    }
    return 0;
}

HashTable *
compare(const char *ref, const char *p, long int k, HashTable *working_table, HashTable *table, HashTable *new_table,
        HashTable *ref_table) {
    char out[k + 1];
    for (int i = 0; i < k; ++i) {
        out[i] = '0';
    }
    out[k] = '\0';
    int count = 0;

    for (int i = 0; i < k; i++) {
        if (ref[i] == p[i]) {
            ht_item *item = ht_get_item(working_table, p[i]);
            out[i] = '+';
            count++;
            item->value--;
        }
    }

    if (count == k) {
        printf("ok");
        return new_table;
    }
    for (int i = 0; i < k; i++) {
        if (ref[i] != p[i]) {
            ht_item *item = ht_get_item(working_table, p[i]);
            if (item != NULL) {
                if (item->value > 0) {
                    out[i] = '|';
                    item->value--;
                    if (item->value == 0) {
                        //todo vincolo 5
                    }
                } else {
                    out[i] = '/';
                }
            } else {
                out[i] = '*';
            }
        }
    }

    first_constraints(out, ref, p, table, new_table, ref_table, working_table);

    printf("%s \n", out);
    printf("%d \n", new_table->count);
    return new_table;
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

void init_insert(HashTable *table, long int k) {
    char p[k];
    fgets(p, 1000, stdin);
    p[strlen(p) - 1] = '\0';
    while (strcmp(p, "+inserisci_fine") != 0) {
        ht_insert(table, p, 0, k);
        fgets(p, 1000, stdin);
        p[strlen(p) - 1] = '\0';
    }
}

void new_round(HashTable *table, long int k) {
    char p[k + 1];
    p[k] = '\0';
    char *out;
    char ref[k];

    fgets(ref, k + 2, stdin);
    ref[k] = '\0';

    HashTable *ref_table = ref_into_hash(ref, k);

    char n_max[10];
    char *ptr;
    long int n;
    fgets(n_max, 5, stdin);
    n_max[(strlen(n_max) - 1)] = '\0';
    n = strtol(n_max, &ptr, 10);
    HashTable *new_table = create_table(table->size);;
    new_table->count = 0;

    int i = 0;
    int bool;

    HashTable *working_table = ref_into_hash(ref, k);


    while (i < n) {
        fgets(p, 1000, stdin);
        if (feof(stdin))
            break;

        if (strcmp(p, "+inserisci_inizio\n") == 0) {
            if (new_table->count >0){
                init_insert(new_table, k);
            }else{
                init_insert(table, k);
            }
        } else if (strcmp(p, "+stampa_filtrate\n") == 0) {
            //todo filtrate
        } else {
            p[k] = '\0';
            if (new_table->count == 0) {
                bool = ht_admitted_search(table, p);
            } else {
                bool = ht_admitted_search(table, p);
            }

            if (bool == 0) {
                printf("not exists \n");

            } else {
                new_table = compare(ref, p, k, working_table, table, new_table, ref_table);

                for (int j = 0; j < 64; j++) {
                    if (ref_table->items[j] != NULL) {
                        working_table->items[j]->value = ref_table->items[j]->value;
                    }
                }
                i++;
            }
        }

        //todo admitted_table= new_table;
    }

    if (i == n) {
        printf("ko \n");
    }
    if (i == n || strcmp(out, "ok") == 0) {  //out non è mai ok al max ++++++++
        for (int j = 0; j < 2; j++) {
            fgets(p, 1000, stdin);
            p[strlen(p) - 1] = '\0';
            if (strcmp(p, "+inserisci_inizio") == 0) {
                init_insert(table, k);
            } else if (strcmp(p, "+nuova_partita") == 0) {
                ric=0;
                free_table(new_table);
                print_table(new_table);
                new_round(table, k);
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
        // Remove this item and set the head of the list
        // as the new item
        free_item(item);
        table->count--;
        LinkedList *node = head;
        head = head->next;
        node->next = NULL;
        table->items[index] = create_item(node->item->key, node->item->value, k);
        free_linkedlist(node);
        table->overflow_buckets[index] = head;

    }
}


int word_search(HashTable *table, char key) { //se c'è ritorno 0
    int x = key;
    int index = ref_into_hash_index(x);
    ht_item *item = table->items[index];

    // Ensure that we move to a non NULL item
    if (item != NULL) {
        if (item->key[0] == key)
            return 0;
    }
    return 1;
}

void first_constraints(char *out, const char *ref, const char *p, HashTable *table, HashTable *new_table,
                       HashTable *ref_table, HashTable *working_table) {
    unsigned long k = strlen(out);
    int i = 0, esc = 0;
    ht_item *item;
    LinkedList *head;
    int x, index;
    HashTable *word_table;

    for (int j = 0; j < table->size; j++) {
        // Ensure that we move to items which are not NULL
        if (ric==0){
            item = table->items[j];
            head = table->overflow_buckets[j];
        }else{
            item = new_table->items[j];
            head = table->overflow_buckets[j];
        }


        if (item != NULL) {
            word_table = ref_into_hash(item->key, strlen(out));

            if (head != NULL) {
                while (item != NULL) {
                    word_table = ref_into_hash(item->key, strlen(out));

                    while (i < k) {
                        x = ref[i];
                        index = ref_into_hash_index(x);
                        if (word_search(word_table, ref_table->items[index]->key[0]) == 0) {
                            if (working_table->items[index]->value == 0) {
                                if (ref_table->items[index]->value == word_table->items[index]->value) {
                                    if (out[i] == '+') {
                                        if (item->key[i] == ref[i]) {
                                            i++;
                                        } else {
                                            esc = 1;
                                            break;
                                        }
                                    } else if (out[i] == '*') {
                                        if (word_search(word_table, p[i]) != 0) {
                                            i++;
                                        } else {
                                            esc = 1;
                                            break;
                                        }
                                    } else if (out[i] == '|') {
                                        if (item->key[i] != ref[i] && word_search(word_table, p[i]) == 0) {
                                            i++;
                                        } else {
                                            esc = 1;
                                            break;
                                        }
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else {
                                if (ref_table->items[index]->value == working_table->items[index]->value) {
                                    if (out[i] == '+') {
                                        if (item->key[i] == ref[i]) {
                                            i++;
                                        } else {
                                            esc = 1;
                                            break;
                                        }
                                    } else if (out[i] == '*') {
                                        if (word_search(word_table, p[i]) != 0) {
                                            i++;
                                        } else {
                                            esc = 1;
                                            break;
                                        }
                                    } else if (out[i] == '|') {
                                        if (item->key[i] != ref[i] && word_search(word_table, p[i]) == 0) {
                                            i++;
                                        } else {
                                            esc = 1;
                                            break;
                                        }
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            }
                        }else {
                            if (working_table->items[index]->value == ref_table->items[index]->value) {
                                if (out[i] == '+') {
                                    if (item->key[i] == ref[i]) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else if (out[i] == '*') {
                                    if (word_search(word_table, p[i]) != 0) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else if (out[i] == '|') {
                                    if (item->key[i] != ref[i] && word_search(word_table, p[i]) == 0) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        }
                    }
                    if (head == NULL) {
                        break;
                    } else {
                        item = head->item;
                        head = head->next;
                    }
                }
                i = 0;
            } else {
                while (i < k) {
                    x = ref[i];
                    index = ref_into_hash_index(x);
                    if (word_search(word_table, ref_table->items[index]->key[0]) == 0) {
                        if (working_table->items[index]->value == 0) {
                            if (ref_table->items[index]->value == word_table->items[index]->value) {
                                if (out[i] == '+') {
                                    if (item->key[i] == ref[i]) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else if (out[i] == '*') {
                                    if (word_search(word_table, p[i]) != 0) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else if (out[i] == '|') {
                                    if (item->key[i] != ref[i] && word_search(word_table, p[i]) == 0) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        } else {
                            if (ref_table->items[index]->value == working_table->items[index]->value) {
                                if (out[i] == '+') {
                                    if (item->key[i] == ref[i]) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else if (out[i] == '*') {
                                    if (word_search(word_table, p[i]) != 0) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else if (out[i] == '|') {
                                    if (item->key[i] != p[i] && word_search(word_table, p[i]) == 0) {
                                        i++;
                                    } else {
                                        esc = 1;
                                        break;
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        }
                    } else {
                        if (working_table->items[index]->value == ref_table->items[index]->value) {
                            if (out[i] == '+') {
                                if (item->key[i] == ref[i]) {
                                    i++;
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else if (out[i] == '*') {
                                if (word_search(word_table, p[i]) != 0) {
                                    i++;
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else if (out[i] == '|') {
                                if (item->key[i] != ref[i] && word_search(word_table, p[i]) == 0) {
                                    i++;
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else {
                                esc = 1;
                                break;
                            }
                        } else {
                            esc = 1;
                            break;
                        }
                    }
                }
            }
            i = 0;
            if (ric==0){
                if (esc == 0) {
                    ht_insert(new_table, item->key, 0, k);
                }
            }else{
                if (esc == 1){
                    ht_delete(new_table, j, item);
                }
            }
        }
        esc = 0;
    }
    ric=1;
}
