#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct { // Define the Hash Table Item here
    char *key;
    char *value;
} ht_item;

typedef struct {
    // Contains an array of pointers to items
    ht_item **items;
    int size;
    int count;
} HashTable;

//ab
HashTable *create_table(int size);

int get_index(unsigned char *str, int size);

unsigned long hash(unsigned char *str);

ht_item* create_item(char *key, char value);

void ht_insert(HashTable* table, char* key, char* value);


int main() {
    int k;
    scanf("%d ", &k);
    unsigned char input_str[k];
    int value_index;
    int init_hash_size = 50;

    HashTable table = *create_table(init_hash_size);

    while (1) {
        fgets(input_str, 1000, stdin);
        input_str[strlen(input_str) - 1] = '\0'; // removing \n at the end

        printf("%s\n", input_str);
        // starting new "game"
        if (strcmp(input_str, "+nuova_partita") == 0) {
            // step 2.

            printf("%s", "starting nuova partita");
            break;
        } else {
            // Create the item
            ht_item* item = create_item(input_str, '0');
            int index = get_index(input_str, init_hash_size);

            HashTable* table = create_table(init_hash_size);

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




            ht_item *item = create_item(input_str, '0');

            // step 1. reading list of legal words
            
        }
    }

    printf("exited");
    return 0;
}

//creates hash item
ht_item *create_item(char *key, char *value) {  //ritorna il puntatore all'item che ho creato
    // Creates a pointer to a new hash table item
    ht_item *item = malloc(sizeof(ht_item));
    item->key = malloc(strlen(key) + 1);
    item->value = malloc(strlen(value) + 1);
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
    return table;
}

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


