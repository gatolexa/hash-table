#ifndef FUNC_H_
#include <stdio.h>
#define FUNC_H_
typedef struct item {
	char* key;
	char* ip;
	int port;
	struct item* prev;
	struct item* next;
}item;
typedef struct dlist {
	item* head;
	item* tail;
}list;
typedef struct hash_table {
	list** items;
	int size;
}table;
item* create_item(char* key, char* ip, int port);
table* create_table();
void free_list(list* thing);
void free_table(table* hash_table);
int getint();
void scanint(int* n);
char* getstr();
char* ipaddress();
int hash_func(char* key);
void insert(table* hash_table, char* key, char* ip, int port);
item* search(table* hash_table, char* key);
void print_search(table* hash_table, char* key);
void print_table(table* hash_table);
void pop(table* hash_table);
void pushback(list* thing, char* key, char* ip, int port);
void menu();
void load(FILE* f, table* hash_table, int* pos);
void createstring(FILE* f, table* hash_table);
#endif