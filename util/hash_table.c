#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

unsigned long hash_function(char* str){
    unsigned long i = 7;
    for(int j = 0; str[j]; j++){
        i = i*31 + str[j];
    }
    return i % HT_CAPACITY;
}

void handle_collisions(hash_table* table, unsigned long index, ht_item* item){

}

ht_item* create_item(char* key, char* value) {
    ht_item* item = (ht_item*) malloc(sizeof(ht_item));
    item->key = (char*) malloc(strlen(key) + 1);
    item->value = (char*) malloc(strlen(value) + 1);

    strcpy(item->key, key);
    strcpy(item->value, value);

    return item;
}

hash_table* create_table(int size){
    hash_table* table = (hash_table*) malloc(sizeof(hash_table));
    table->size = size;
    table->count = 0;
    table->items = (ht_item**) calloc(table->size, sizeof(ht_item*));
    for(int i = 0; i < table->size; i++){
        table->items[i] = NULL;
    }

    return table;
}

void free_item(ht_item* item){
    free(item->key);
    free(item->value);
    free(item);
}

void free_table(hash_table* table){
    for(int i = 0; i < table->size; i++){
        ht_item* item = table->items[i];
        if(item != NULL){
            free_item(item);
        }
    }
    free(table->items);
    free(table);
}

void ht_insert(hash_table* table, char* key, char* value){
    ht_item* item = create_item(key, value);

    unsigned long index = hash_function(key);

    ht_item* current_item = table->items[index];

    if(current_item == NULL){
        if(table->count == table->size){
            printf("Insert ErrorL Hash Table is full\n");
            free_item(item);
            return;
        }

        table->items[index] = item;
        table->count++;
    } else {
        if(strcmp(current_item->key, key) == 0) {
            strcpy(table->items[index]->value, value);
            return;
        } else {
            handle_collisions(table, index, item);
            return;
        }
    }
}

char* ht_search(hash_table* table, char* key){
    int index = hash_function(key);
    ht_item* item = table->items[index];

    if(item != NULL){
        if(strcmp(item->key, key) == 0){
            return item->value;
        }
    }
    return NULL;
}
