#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sltm.h"

static Event clone_event(const Event *e){
    Event r = *e;
    r.id      = xstrdup(e->id);
    r.vehicle = xstrdup(e->vehicle);
    r.mission = xstrdup(e->mission);
    r.site    = xstrdup(e->site);
    return r;
}

Node* list_insert_sorted(Node *head, Event e){
    Node *n = malloc(sizeof(Node));
    if(!n){ perror("malloc"); exit(1); }
    n->e = clone_event(&e);
    n->next = NULL;

    if(!head || date_cmp(e.date, head->e.date) < 0){
        n->next = head;
        return n;
    }
    Node *cur = head;
    while(cur->next && date_cmp(cur->next->e.date, e.date) <= 0){
        cur = cur->next;
    }
    n->next = cur->next;
    cur->next = n;
    return head;
}

bool list_delete_by_id(Node **phead, const char *id){
    Node *cur = *phead, *prev = NULL;
    while(cur){
        if(strcmp(cur->e.id, id)==0){
            if(prev) prev->next = cur->next; else *phead = cur->next;
            free(cur->e.id); free(cur->e.vehicle); free(cur->e.mission); free(cur->e.site);
            free(cur);
            return true;
        }
        prev = cur; cur = cur->next;
    }
    return false;
}

Node* list_find_by_id(Node *head, const char *id){
    for(Node *p=head; p; p=p->next)
        if(strcmp(p->e.id,id)==0) return p;
    return NULL;
}

void list_print_range(Node *head, Date a, Date b){
    for(Node *p=head; p; p=p->next){
        if(date_in_range(p->e.date, a, b)){
            printf("%s,%04d-%02d-%02d,%s,%s,%s,%s\n",
                p->e.id, p->e.date.year, p->e.date.month, p->e.date.day,
                p->e.vehicle, p->e.mission, p->e.site, status_to_str(p->e.status));
        }
    }
}

void list_free(Node *head){
    while(head){
        Node *n=head->next;
        free(head->e.id); free(head->e.vehicle); free(head->e.mission); free(head->e.site);
        free(head);
        head=n;
    }
}
