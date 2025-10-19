#ifndef SLTM_H
#define SLTM_H

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    ST_SCHEDULED,
    ST_SUCCESS,
    ST_FAILURE,
    ST_DELAYED,
    ST_CANCELLED,
    ST_INVALID
} Status;

typedef struct {
    int year, month, day; // ISO 8601 date parts
} Date;

typedef struct Event {
    char *id;
    Date  date;
    char *vehicle;
    char *mission;
    char *site;
    Status status;
} Event;

typedef struct Node {
    Event e;
    struct Node *next;
} Node;

/* list.c */
Node*  list_insert_sorted(Node *head, Event e);  // by date asc, stable
bool   list_delete_by_id(Node **phead, const char *id);
Node*  list_find_by_id(Node *head, const char *id);
void   list_print_range(Node *head, Date a, Date b);
void   list_free(Node *head);

/* csv.c */
int    csv_load_file(const char *path, Node **phead, int *ignored);
bool   csv_export_file(const char *path, Node *head);

/* date.c */
bool   date_parse_ymd(const char *s, Date *out);         // "YYYY-MM-DD"
int    date_cmp(Date a, Date b);                         // <0,0,>0
bool   date_in_range(Date x, Date a, Date b);            // inclusive

/* util.c */
char*  xstrdup(const char *s);
char*  str_trim(char *s);
Status status_from_str(const char *s);
const char* status_to_str(Status st);
bool   contains_case_insensitive(const char *hay, const char *needle);

#endif
