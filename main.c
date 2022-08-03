#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct { // Define the Hash Table Item here
    char *key;
    char *value;
} ht_item;

typedef struct LinkedList LinkedList;

struct LinkedList {
    ht_item*  item;
    LinkedList* next;
};

typedef struct {
    // Contains an array of pointers to items
    ht_item **items;
    LinkedList** overflow_buckets;
    int size;
    int count;
} HashTable;


HashTable* create_table(int size);

int get_index(unsigned char *str, int size);

unsigned long hash(unsigned char *str);

ht_item* create_item(char *key, char *value, long int k);

void ht_insert(HashTable* table, char* key, char* value, int size, long int k);

void resize(int size, HashTable* table, long int k );

void handle_collision(HashTable* table, unsigned long index, ht_item* item);

void print_table(HashTable* table);

HashTable* ref_into_hash(char* str, long int k);

int ref_into_hash_index (int char_to_int_letter, long int k);

char* compare (const char* ref, const char* p, long int k, HashTable* table);

int ht_admitted_search(HashTable* table, char* key, int size);

int ht_ref_search (HashTable* table, char key, long int k);

int main() {
    long int k;
    char c[2]; //I need it for fgets
    char *ptr; //This is the reference to an object of type char*, whose value is set by the function to the next character in str after the numerical value.
    printf("inserici la lunghezza delle parole: ");
    fgets (c, 4, stdin);
    printf("%s", c);
    k = strtol(c, &ptr, 10);
    printf("%ld ciao\n", k);
    char input_str[k];
    int init_hash_size = 53;
    char val[] = "0";
    char ref[k], p[k];
    char* out;
    long int n;
    HashTable* ref_table;

    HashTable* table = create_table(init_hash_size);
    printf("inserisci parole ammissibili \n");
    while (1) {
        fgets(input_str, 1000, stdin);
        input_str[strlen(input_str) - 1] = '\0'; // removing \n at the end

        // starting new "game"

        if (strcmp(input_str, "+nuova_partita") == 0) {
            // step 2.
            printf("%s", "starting nuova partita");
            break;

        } else { // step 1. reading list of legal words
            if ((strlen(input_str)) != k) {
                printf("Parola troppo lunga/corta \n");
            } else {
                // Inserting each string in my hash table
                ht_insert(table, input_str, val, init_hash_size, k);
            }
        }
    }
    print_table(table);

    printf("inserisci la parola di riferimento: ");
    fgets(ref, k+2, stdin);
    ref[strlen(ref) - 1] = '\0';
    ref_table = ref_into_hash(ref, k);

    printf("inserisci numero massimo di parole da confrontare: ");
    fgets (c, 4, stdin);
    n = strtol(c, &ptr, 10);

    for (int i = 0; i < n; i++) {
        fgets(p, k+2, stdin);
        p[strlen(p) - 1] = '\0';
        int ok = ht_admitted_search(table, p, init_hash_size);
        printf("%d", ok);
        if (ok == 0) {
            printf("not_exists \n");
        }
        if (ok ==1){
            printf("siamo nell'else");
            out = compare (ref, p, k, ref_table);
            printf("%s end \n", out);
        }

    }

    return 0;
}


//creates hash item
ht_item *create_item(char *key, char *value, long int k) {  //ritorna il puntatore all'item che ho creato
    // Creates a pointer to a new hash table item
    ht_item *item = (ht_item *) malloc(sizeof(ht_item));
    item->key = (char*) malloc(k);
    item->value = (char*) malloc(k);
    strcpy(item->key, key);
    strcpy(item->value, value);
    return item;
}

/** Creates empty hash table
 *
 * {
 *      key1: value1,
 *      key2: value2
 * }
 * index_val1 = hash(key1)
 * value1 = items[index_val1]
 */

//mod n so that the index is actually in the table
int get_index(unsigned char *str, int size) {
    return hash(str) % size;
}

//proper hash function
unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void ht_insert(HashTable* table, char* key, char* value, int size, long int k){
    ht_item* item = create_item(key, value, k);        //create an item
    int index = get_index(key, size);          //apply hash function

    ht_item* current_item = table->items[index];
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
    }
    else {
        // Scenario 1: We only need to update value
        if (strcmp(current_item->key, key) == 0) {
            strcpy(table->items[index]->value, value);
            return;
        }
        else {
            // Scenario 2: Collision
            // We will handle case this a bit later
            handle_collision(table, index, item);
            return;
        }
    }
}

void resize(int size, HashTable* table, long int k ) {

    int oldTableSize = size;
    HashTable* new_table;

    size = oldTableSize * 2;
    new_table = create_table(size);

    for (int i = 0; i < size; i++)
        new_table->items[i] = NULL;

    size = 0;

    for (int hash = 0; hash < oldTableSize; hash++)
        if (table->items [hash]  != NULL) {
            ht_insert(new_table, table->items[hash]->key, table->items[hash]->value, size, k);
        }

    for (int i = 0; i <= oldTableSize; ++i) {
        free(table->items[i]);
    }
}

static LinkedList* allocate_list () {
    // Allocates memory for a Linkedlist pointer
    LinkedList* list = (LinkedList*) malloc (sizeof(LinkedList));
    return list;
}

static LinkedList* linkedlist_insert(LinkedList* list, ht_item* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList* head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    }

    else if (list->next == NULL) {
        LinkedList* node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }

    LinkedList* node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;

    return list;
}

static ht_item* linkedlist_remove(LinkedList* list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList* node = list->next;
    LinkedList* temp = list;
    temp->next = NULL;
    list = node;
    ht_item* it = NULL;
    memcpy(temp->item, it, sizeof(ht_item));
    free(temp->item ->key);
    free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}

static void free_linkedlist(LinkedList* list) {
    LinkedList* temp = list;
    while (list) {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}

static LinkedList** create_overflow_buckets(HashTable* table) {
    // Create the overflow buckets; an array of linkedlists
    LinkedList** buckets = (LinkedList**) calloc (table->size, sizeof(LinkedList*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}

static void free_overflow_buckets(HashTable* table) {
    // Free all the overflow bucket lists
    LinkedList** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
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
    table->overflow_buckets = create_overflow_buckets(table);
    return table;
}

void free_item(ht_item* item) {
    // Frees an item
    free(item->key);
    free(item->value);
    free(item);
}

void free_table(HashTable* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        ht_item* item = table->items[i];
        if (item != NULL)
            free_item(item);
    }

    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void handle_collision(HashTable* table, unsigned long index, ht_item* item) {
    LinkedList* head = table->overflow_buckets[index];

    if (head == NULL) {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = linkedlist_insert(head, item);
        return;
    }
}


void print_table(HashTable* table) {
    printf("\nHash Table\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%s, Value:%s\n", i, table->items[i]->key, table->items[i]->value);
        }
    }
    printf("-------------------\n\n");
}

HashTable* ref_into_hash(char* str, long int k){
    int index, x;
    char val[] = "1";

    HashTable* table = create_table(64);
    for (int i = 0; i < k; i++) {
        x = str[i];
        index = ref_into_hash_index (x, k);

        char c = str[i];
        char* key = &c;
        ht_item* item = create_item(key, val, k);
        ht_item* current_item = table->items[index];
        if (current_item == NULL) {
            // Key does not exist.
            // Insert directly
            table->items[index] = item;
            table->count++;
        }
        else {
            // key exists: we only need to update value
                int value = (*(table->items[index]->value) - '0') + 1;
                printf("%d", value);
                sprintf(table->items[index]->value, "%d", value); //converting int into char*
                printf("%s", table->items[index]->value);
        }
    }
    return table;
}

int ref_into_hash_index (int char_to_int_letter, long int k){
    int index;

        if (char_to_int_letter>=65 && char_to_int_letter <= 90 ){
            index = char_to_int_letter % 65;
        }
        if (char_to_int_letter>=48 && char_to_int_letter <= 57){
            index = char_to_int_letter + 5;
        }
        if (char_to_int_letter>=97 && char_to_int_letter <= 122){
            index = char_to_int_letter - 70;
        }
        if (char_to_int_letter == 95){
            index = 26;
        }
        if (char_to_int_letter == 45){
            index = 63;
        }
        return index;
}

int ht_admitted_search(HashTable* table, char* key, int size) {
    // Searches the key in the hashtable and returns 1 if it exists, 0 otherwise
    int index = get_index (key, size);
    ht_item* item = table->items[index];

    if (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return 1;
    }
    return 0;
}

int ht_ref_search (HashTable* table, char key, long int k){
    int x = key;
    int index = ref_into_hash_index(x,k);
    ht_item* item = table->items[index];

    if (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return 1;
    }
    return 0;

}

char* compare (const char* ref, const char* p, long int k, HashTable* table){
    char out[k];
    int count=0;
    for (int i = 0; i < k; i++) {
        if (ref[i]==p[i]){
           out[i]='+';
           count++;
        }
    }

    printf("%d", count);

    if (count==k){
        printf("ok");
        return out;
    }
    for (int i = 0; i < k; i++) {
        if (ht_ref_search(table, p[i], k) == 1 && table->items[i]->value>0){
            out[i]= '|';
            int value = (*(table->items[i]->value) - '0') - 1;
            sprintf(table->items[i]->value, "%d", value);
        }

        else {
            out[i]= '/';
        }
    }

    return out;
}