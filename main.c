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

int
first_constraints(char *out, int i, ht_item *item, char *must_be, char *ref, int esc, HashTable *word_table, char *p,
                  HashTable **arr_constr);

void
constraints(char *out, char *ref, char *p, HashTable *table, HashTable *new_table, HashTable *ref_table,
            HashTable *working_table, HashTable *hash_constr_table, char *must_be, HashTable **arr_constr);

void ht_delete(HashTable *table, int index, ht_item *item);

void new_round(HashTable *table, long int k);

HashTable *create_table(int size);

int get_index(unsigned char *str, int size);

unsigned long hash(unsigned char *str);

ht_item *create_item(char *key, int value, long int k);

void ht_insert(HashTable *table, char *key, int value, long int k);

HashTable *resize(int size, HashTable *table, long int k);

void handle_collision(HashTable *table, unsigned long index, ht_item *item);

void print_table(HashTable *table);

HashTable *word_into_table(const char *str);

int ref_into_hash_index(int char_to_int_letter);

HashTable *
compare(char *ref, char *p, long int k, HashTable *working_table, HashTable *table, HashTable *new_table,
        HashTable *ref_table, HashTable **arr_constrs, char *must_be, HashTable *hash_constr_table,
        char *out);

int ht_admitted_search(HashTable *table, char *key);

ht_item *ht_get_item(HashTable *table, char key);

int word_search(HashTable *table, char key);

void init_insert(HashTable *table, long int k, HashTable **arr_constr, char *must_be, HashTable *hash_ref_constrs,
                 HashTable *admitted_table);

LinkedList *LinkedList_delete(LinkedList *head, char *key);

int main() {
    //freopen("output.txt", "w+", stdout);
    //freopen("slide.txt", "r", stdin);
    long int k;
    char *ptr;
    char c[2];//I need it for fgets
    fgets(c, 4, stdin);
    c[strlen(c) - 1] = '\0';
    k = strtol(c, &ptr, 10);
    char input_str[k];
    int init_hash_size = 101;
    int i = 0;

    HashTable *table = create_table(init_hash_size);
    table->count = 0;

    while (1) {
        fgets(input_str, 1000, stdin);
        input_str[strlen(input_str) - 1] = '\0'; // removing \n at the end

        // starting new "game"
        if (strcmp(input_str, "+nuova_partita") == 0) {
            // step 2.
            print_table(table);
            new_round(table, k);
            break;

        } else { // step 1. reading list of legal words
            // Inserting each string in my hash table
            if (table->count == round((init_hash_size * 0.7))) {
                print_table(table);
                table = resize(init_hash_size, table, k);
                init_hash_size = (init_hash_size * 2);
                printf("%d", init_hash_size);
                print_table(table);
            }
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

HashTable *resize(int size, HashTable *table, long int k) {

    int oldTableSize = size;
    HashTable *new_table;
    int index;

    size = (oldTableSize * 2) + 1;
    new_table = create_table(size);

    for (int i = 0; i < oldTableSize; i++) {
        if (table->items[i] != NULL) {
            index = get_index(table->items[i]->key, size);
            new_table->items[index] = table->items[i];
            if (table->overflow_buckets[i] != NULL) {
                new_table->overflow_buckets[index]=table->overflow_buckets[i];
            }
        }
    }
    free(table);
    return new_table;
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
    } else {
        LinkedList *temp = list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = NULL;
        temp->next = node;
        return list;
    }
}

/*static ht_item *linkedlist_remove(LinkedList *list) {
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
}*/

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

HashTable *compare(char *ref, char *p, long int k, HashTable *working_table, HashTable *table, HashTable *new_table,
                   HashTable *ref_table, HashTable **arr_constrs, char *must_be, HashTable *hash_constr_table,
                   char *out) {

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
        out[0] = 'o';
        out[1] = 'k';
        out[2] = '\0';
        printf("ok\n");
        return new_table;
    }
    for (int i = 0; i < k; i++) {
        if (ref[i] != p[i]) {
            ht_item *item = ht_get_item(working_table, p[i]);
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

    constraints(out, ref, p, table, new_table, ref_table, working_table, hash_constr_table, must_be, arr_constrs);

    for (int i = 0; i < k; ++i) {
        if (out[i] == '*') {
            out[i] = '/';
        }
    }
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

void init_insert(HashTable *table, long int k, HashTable **arr_constr, char *must_be, HashTable *hash_ref_constrs,
                 HashTable *admitted_table) {

    char p[k];
    fgets(p, 1000, stdin);
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
                        if (hash_ref_constrs->items[index] != NULL) {
                            if (hash_ref_constrs->items[index]->value == word_table->items[index]->value) {
                                ht_insert(table, p, 0, k);
                            }
                        }
                    }
                }
            } else {

                if (word_search(arr_constr[i], p[i]) != 0) {
                    int x = p[i];
                    int index = ref_into_hash_index(x);
                    if (hash_ref_constrs->items[index] != NULL) {
                        if (hash_ref_constrs->items[index]->value == word_table->items[index]->value) {
                            ht_insert(table, p, 0, k);
                        }
                    }
                }
            }
        }
        fgets(p, 1000, stdin);
        p[strlen(p) - 1] = '\0';
    }
}

void new_round(HashTable *table, long int k) {
    char p[k + 1], n_max[10];
    p[k] = '\0';
    char *ptr;
    char ref[k];
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
        arr_constr[i]->count = 0;
    }

    fgets(ref, k + 2, stdin);
    ref[k] = '\0';

    HashTable *ref_table = word_into_table(ref);

    long int n;
    fgets(n_max, 5, stdin);
    n_max[(strlen(n_max) - 1)] = '\0';
    n = strtol(n_max, &ptr, 10);
    HashTable *new_table = create_table(table->size);;
    new_table->count = 0;

    int i = 0;
    int bool;

    HashTable *working_table = word_into_table(ref);

    while (i < n) {
        fgets(p, 1000, stdin);
        if (feof(stdin))
            break;

        if (strcmp(p, "+inserisci_inizio\n") == 0) {
            if (new_table->count > 0) {
                init_insert(new_table, k, arr_constr, must_be, hash_constr_table, table);
            } else {
                init_insert(table, k, arr_constr, must_be, hash_constr_table, table);
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
                printf("not_exists \n");

            } else {
                new_table = compare(ref, p, k, working_table, table, new_table, ref_table, arr_constr, must_be,
                                    hash_constr_table, out);
                if (strcmp(out, "ok") == 0) {
                    break;
                }

                for (int j = 0; j < 64; j++) {
                    if (ref_table->items[j] != NULL) {
                        working_table->items[j]->value = ref_table->items[j]->value;
                    }
                }
                i++;
            }
        }
    }


    if (i == n && strcmp(out, "ok") != 0) {
        printf("ko \n");
    }
    if (i == n || strcmp(out, "ok") == 0) {
        for (int j = 0; j < 2; j++) {
            fgets(p, 1000, stdin);
            p[strlen(p) - 1] = '\0';
            if (strcmp(p, "+inserisci_inizio") == 0) {
                init_insert(table, k, arr_constr, must_be, hash_constr_table, table);
            } else if (strcmp(p, "+nuova_partita") == 0) {
                ric = 0;
                free_table(new_table);
                j = 2;
                new_round(table, k);
            }
        }
    }
    return;
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
    int i = 0, esc = 0, x, index;
    ht_item *item, *constr_items;
    LinkedList *head;
    HashTable *word_table;
    char ref_i[2] = "\0";

    for (int j = 0; j < table->size; j++) {
        // Ensure that we move to items which are not NULL
        if (ric == 0) {
            item = table->items[j];
            head = table->overflow_buckets[j];
        } else {
            item = new_table->items[j];
            head = new_table->overflow_buckets[j];
        }

        if (item != NULL) {
            word_table = word_into_table(item->key);


            if (head != NULL) {
                while (item != NULL) {

                    word_table = word_into_table(item->key);

                    while (i < k) {
                        x = ref[i];
                        index = ref_into_hash_index(x);
                        if (word_search(word_table, ref_table->items[index]->key[0]) == 0) {
                            if (working_table->items[index]->value == 0) {
                                if (ref_table->items[index]->value == word_table->items[index]->value) {
                                    if (hash_constr_table->items[index] == NULL) {
                                        ref_i[0] = ref[i];
                                        constr_items = create_item(ref_i, ref_table->items[index]->value, k);
                                        hash_constr_table->items[index] = constr_items;
                                        hash_constr_table->count++;
                                    } else {
                                        if (hash_constr_table->items[index]->value < word_table->items[index]->value) {
                                            hash_constr_table->items[index]->value = ref_table->items[index]->value;
                                        }
                                    }
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
                                    i++;
                                    if (esc == 1) {
                                        break;
                                    }
                                } else {
                                    esc = 1;
                                    break;
                                }
                            } else {
                                if (ref_table->items[index]->value == working_table->items[index]->value) {
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
                                    i++;
                                    if (esc == 1) {
                                        break;
                                    }
                                } else if ((ref_table->items[index]->value - working_table->items[index]->value) ==
                                           //numero minimo di caratteri
                                           word_table->items[index]->value) {
                                    if (hash_constr_table->items[index] == NULL) {
                                        ref_i[0] = ref[i];
                                        constr_items = create_item(ref_i, ref_table->items[index]->value, k);
                                        hash_constr_table->items[index] = constr_items;
                                        hash_constr_table->count++;
                                    } else {
                                        if (hash_constr_table->items[index]->value < word_table->items[index]->value) {
                                            hash_constr_table->items[index]->value = ref_table->items[index]->value;
                                        }
                                    }
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
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
                            if (working_table->items[index]->value == ref_table->items[index]->value) {
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
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
                }
            } else {
                while (i < k) {
                    x = ref[i];
                    index = ref_into_hash_index(x);
                    if (word_search(word_table, ref_table->items[index]->key[0]) == 0) {
                        if (working_table->items[index]->value == 0) {
                            if (ref_table->items[index]->value == word_table->items[index]->value) {
                                if (hash_constr_table->items[index] == NULL) {
                                    ref_i[0] = ref[i];
                                    constr_items = create_item(ref_i, ref_table->items[index]->value, k);
                                    hash_constr_table->items[index] = constr_items;
                                    hash_constr_table->count++;
                                } else {
                                    if (hash_constr_table->items[index]->value < word_table->items[index]->value) {
                                        hash_constr_table->items[index]->value = ref_table->items[index]->value;
                                    }
                                    //hash_constr_table->items[index]->value = ref_table->items[index]->value;
                                }
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
                                i++;
                                if (esc == 1) {
                                    break;
                                }
                            } else {
                                if (ref_table->items[index]->value == working_table->items[index]->value) {
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
                                    i++;
                                    if (esc == 1) {
                                        break;
                                    }
                                } else if ((ref_table->items[index]->value - working_table->items[index]->value) ==
                                           //numero minimo di caratteri
                                           word_table->items[index]->value) {
                                    if (hash_constr_table->items[index] == NULL) {
                                        ref_i[0] = ref[i];
                                        constr_items = create_item(ref_i, ref_table->items[index]->value, k);
                                        hash_constr_table->items[index] = constr_items;
                                        hash_constr_table->count++;
                                    } else {
                                        if (hash_constr_table->items[index]->value < word_table->items[index]->value) {
                                            hash_constr_table->items[index]->value = ref_table->items[index]->value;
                                        }
                                    }
                                    esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
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
                            if (working_table->items[index]->value == ref_table->items[index]->value) {
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
                                i++;
                                if (esc == 1) {
                                    break;
                                }
                            } else if ((ref_table->items[index]->value - working_table->items[index]->value) ==
                                       //numero minimo di caratteri
                                       word_table->items[index]->value) {
                                if (hash_constr_table->items[index] == NULL) {
                                    ref_i[0] = ref[i];
                                    constr_items = create_item(ref_i, ref_table->items[index]->value, k);
                                    hash_constr_table->items[index] = constr_items;
                                    hash_constr_table->count++;
                                } else {
                                    if (hash_constr_table->items[index]->value < word_table->items[index]->value) {
                                        hash_constr_table->items[index]->value = ref_table->items[index]->value;
                                    }
                                }
                                esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
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
                        if (working_table->items[index]->value == ref_table->items[index]->value) {
                            esc = first_constraints(out, i, item, must_be, ref, 0, word_table, p, arr_constr);
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
            }
        }
    }
    ric = 1;
}

int first_constraints(char *out, int i, ht_item *item, char *must_be, char *ref, int esc, HashTable *word_table,
                      char *p, HashTable **arr_constr) {
    int k = strlen(ref);
    if (out[i] == '+') {
        // letter that MUST appear at p[i]
        if (item->key[i] == ref[i]) {
            must_be[i] = ref[i];
        } else {
            esc = 1;
        }
    } else if (out[i] == '*') {
        // letter that CANNOT appear anywhere in p
        if (word_search(word_table, p[i]) != 0) {// if p[i] not in parola ammissibile
            for (int l = 0; l < k; l++) {
                if (word_search(arr_constr[l], p[i]) != 0) {
                    // create table
                    ht_item *new_item = (ht_item *) malloc(sizeof(ht_item));
                    new_item->key = (char *) malloc(k);
                    new_item->key[0] = p[i];
                    new_item->value = 1;
                    int index = ref_into_hash_index((int) p[i]);
                    arr_constr[l]->items[index] = new_item;
                    arr_constr[l]->count++;
                }
            }
        } else {
            esc = 1;
        }
    } else if (out[i] == '|') {
        // letter that CANNOT appear at p[i]
        if (item->key[i] != p[i] && word_search(word_table, p[i]) == 0) {
            if (arr_constr[i]->count == 0) {
                // create table
                arr_constr[i] = word_into_table(&p[i]);
            } else if (word_search(arr_constr[i], p[i]) != 0) {
                // insert into existing table
                ht_item *new_item = (ht_item *) malloc(sizeof(ht_item));
                new_item->key = (char *) malloc(k);
                new_item->key[0] = p[i];
                new_item->value = 1;
                int index = ref_into_hash_index((int) p[i]);
                arr_constr[i]->items[index] = new_item;
                arr_constr[i]->count++;
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