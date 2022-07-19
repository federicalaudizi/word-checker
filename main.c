#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct { // Define the Hash Table Item here
    char *key;
    char *value;
} Ht_item;

typedef struct {
    // Contains an array of pointers to items
    Ht_item **items;
    int size;
    int count;
} HashTable;

HashTable *create_table(int size);
unsigned long hashfun(unsigned char *str)
Ht_item *create_item(char *key, char *value);


int main() {
    int k;
    scanf("%d ", &k);
    unsigned char input_str[k];
    unsigned char value;
    int size = 50;

    HashTable *table = *create_table(size);

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
            // step 1. reading list of legal words

            value = hashfun(input_str);
            create_item(input_str, value);
            break;
        }
    }

    printf("exited");
    return 0;
}

Ht_item *create_item(char *key, char *value) {  //ritorna il puntatore all'item che ho creato
    // Creates a pointer to a new hash table item
    Ht_item *item = malloc(sizeof(Ht_item));
    item->key = malloc(strlen(key) + 1);
    item->value = malloc(strlen(value) + 1);
    strcpy(item->key, key);
    strcpy(item->value, value);
    return item;
}

HashTable *create_table(int size) {
    // Creates a new HashTable
    HashTable *table = (HashTable *) malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    // dynamic array of pointers to items
    table->items = calloc(table->size, sizeof(Ht_item *));
    for (int i = 0; i < table->size; i++) //sets all it’s items to NULL (Since they aren’t used)
        table->items[i] = NULL;
    return table;
}

unsigned long hashfun(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}