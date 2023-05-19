#define _CRT_SECURE_NO_WARNINGS
#include "func.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CAP 100000
void menu() {
	enum select { add = 1, create, deleteitem, search, print };
	int pos = 0;
	FILE* f;
	int choose = 0;
	f = fopen("source.txt", "r");
	srand(time(NULL));
	table* hash_table = create_table();
	do {
		printf("choose action:\n1.add strings\n2.create new string\n3.delete\n4.search\n5.print\n0.exit\n");
		do {
			scanint(&choose);
		} while (choose < 0 || choose>5);
		switch (choose) {
		case add:
			load(f, hash_table, &pos);
			break;
		case create:
			createstring(f, hash_table);
			break;
		case deleteitem:
			if (hash_table->size) pop(hash_table);
			else printf("empty table.\n");
			break;
		case search:
		{
			if (hash_table->size) {
				printf("enter name: ");
				char* name;
				name = getstr();
				print_search(hash_table, name);
			}
			else printf("empty table.\n");
		}
		break;
		case print:
			if (hash_table->size) print_table(hash_table);
			else printf("empty table.\n");
			break;
		}
	} while (choose != 0);
	fclose(f);
}
void load(FILE* f, table* hash_table, int* pos) {
	float time = 0;
	int n = 0;
	char* name = (char*)calloc(30, 1);
	char* ip = (char*)calloc(30, 1);
	char* port = (char*)calloc(7, 1);
	printf("enter number of strings: ");
	do {
		scanint(&n);
	} while (n < 0);
	fseek(f, *pos, SEEK_SET);
	for (int i = 0; i < n && !feof(f); i++) {
		if (fscanf(f, "%s", name)) {
			name[strlen(name)] = '\0';
			fscanf(f, "%s", ip);
			ip[strlen(ip)] = '\0';
			fscanf(f, "%s", port);
			time=insert(hash_table, name, ip, atoi(port),time);
		}
	}
	printf("load time: %.4f\n", time / 1000.0);
	*pos = ftell(f);
}
void createstring(FILE* f, table* hash_table) {
	char* key;
	char choose;
	char* ip;
	int port;
	printf("enter name (max 30 symbols): ");
	key = getstr();
	printf("random ip and port (y/n): ");
	do {
		rewind(stdin);
		scanf("%c", &choose);
	} while (choose != 'y' && choose != 'n');
	if (choose == 'y') {
		ip = ipaddress();
		port = rand() % 65535;
	}
	else {
		printf("enter ip and port: ");
		ip = getstr();
		do {
			scanint(&port);
		} while (port < 0 || port>65535);
	}
	fseek(f, 0, SEEK_END);
	fprintf(f, "%s %s %d\n", key, ip, port);
	insert(hash_table, key, ip, port,0);
}
int getint() {
	char temp;
	while ((temp = getchar()) != '\n') {
		if (temp > '9' || temp < '0') return 0;
	}
	return 1;
}
void scanint(int* n) {
	do {
		rewind(stdin);
		scanf_s("%d", n);
	} while (getint() != 1);
}
char* getstr() {
	rewind(stdin);
	char* str = (char*)calloc(30, 1);
	fgets(str, 30, stdin);
	str[strlen(str) - 1] = '\0';
	return str;
}
char* ipaddress() {
	char* temp = (char*)calloc(20, 1);
	int part;
	char* str = (char*)calloc(4, 1);
	int count = 0;
	for (int i = 0; i < 4; i++) {
		part = rand() % 255;
		_itoa_s(part, str, 5, 10);
		for (int j = 0; j < strlen(str); j++) {
			temp[count] = str[j];
			count++;
		}
		temp[count] = '.';
		count++;
	}
	temp[count - 1] = '\0';
	return temp;
}
item* create_item(char* key, char* ip, int port) {
	item* thing = (item*)malloc(sizeof(item));
	thing->ip = (char*)calloc(30, 1);
	strcpy(thing->ip, ip);
	thing->port = port;
	return thing;
}
table* create_table() {
	table* hash_table = (table*)calloc(1, sizeof(table));
	hash_table->items = (list**)calloc(CAP, sizeof(list*));
	for (int i = 0; i < CAP; i++) {
		hash_table->items[i] = (list*)calloc(1, sizeof(list));
		hash_table->items[i]->head = NULL;
		hash_table->items[i]->tail = NULL;
	}
	hash_table->size = 0;
	return hash_table;
}
void free_list(list* thing) {
	item* temp = thing->head;
	item* next = NULL;
	while (temp) {
		next = temp->next;
		free(temp->key);
		free(temp->ip);
		free(temp);
		temp = next;
	}
	free(thing);
}
void free_table(table* hash_table) {
	for (int i = 0; i < CAP; i++) {
		if (hash_table->items[i]) free_list(hash_table->items[i]);
	}
	free(hash_table->items);
	free(hash_table);
}
int hash_func(char* key) {
	int hash = 1;
	for (int i = 0; i < strlen(key); i++) {
		hash = (hash * 67321 + (int)key[i]) % CAP;
	}
	if (hash < 0) hash = -hash;
	return hash;
}
float insert(table* hash_table, char* key, char* ip, int port,float time) {
	item* temp;
	clock_t start = clock();
	int hash = hash_func(key);
	item* thing = create_item(key, ip, port);
	if (temp = search(hash_table, key)) {
		strcpy(temp->ip, thing->ip);
		temp->port = thing->port;
	}
	else {
		hash_table->size++;
		pushback(hash_table->items[hash], key, thing->ip, thing->port);
	}
	clock_t end = clock();
	return end - start + time;
}
item* search(table* hash_table, char* key) {
	int hash = hash_func(key);
	for (item* temp = hash_table->items[hash]->head; temp; temp = temp->next) {
		if (strcmp(key, temp->key) == 0) return temp;
	}
	return NULL;
}
void print_search(table* hash_table, char* key) {
	clock_t start = clock();
	item* thing = search(hash_table, key);
	clock_t end = clock();
	printf("runtime: %.4f\n", (end - start) / 1000.0);
	if (thing) printf("name: %s, ip: %s, port: %d\n", key, thing->ip, thing->port);
	else(printf("not found.\n"));
}
void print_table(table* hash_table) {
	int count = 0;
	printf("#   name       ip          port\n");
	printf("___________________________________\n");
	for (int i = 0; i < CAP; i++) {
		if (hash_table->items[i]) {
			item* temp = hash_table->items[i]->head;
			for (; temp; temp = temp->next) {
				count++;
				printf("%d. %s, %s, %d\n", count, temp->key, temp->ip, temp->port);
			}
		}
	}
	printf("___________________________________\n");
}
void pop(table* hash_table) {
	char* key;
	int flag = 0;
	printf("enter name: ");
	key = getstr();
	int hash = hash_func(key);
	for (item* temp = hash_table->items[hash]->head; temp; temp = temp->next) {
		printf("1");
		if (temp && strcmp(key, temp->key) == 0) {
			if (temp->prev) temp->prev->next = temp->next;
			else hash_table->items[hash]->head = temp->next;
			if (temp->next) temp->next->prev = temp->prev;
			else hash_table->items[hash]->tail = temp->prev;
			free(temp);
			flag = 1;
			break;
		}
	}
	if (!flag) printf("not found.\n");
}
void pushback(list* thing, char* key, char* ip, int port) {
	item* temp = (item*)calloc(1, sizeof(item));
	temp->next = NULL;
	temp->prev = thing->tail;
	temp->ip = (char*)calloc(strlen(ip) + 1, 1);
	temp->key = (char*)calloc(strlen(key) + 1, 1);
	strcpy(temp->ip, ip);
	strcpy(temp->key, key);
	temp->port = port;
	if (thing->tail) thing->tail->next = temp;
	if (!thing->head) thing->head = temp;
	thing->tail = temp;
}
