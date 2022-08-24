#define HT_CAPACITY 50000

typedef struct ht_item ht_item;
struct ht_item{
    char* key;
    char* value;
};

typedef struct hash_table hash_table;
struct hash_table{
    ht_item** items;
    int size;
    int count;
};

ht_item* create_item(char* key, char* value);
hash_table* create_table(int size);

void free_item(ht_item* item);
void free_table(hash_table* table);

void ht_insert(hash_table* table, char* key, char* value);
char* ht_search(hash_table* table, char* key);


