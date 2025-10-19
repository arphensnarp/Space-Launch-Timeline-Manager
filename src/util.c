#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "sltm.h"

char* xstrdup(const char *s){
    if(!s) return NULL;
    size_t n = strlen(s)+1;
    char *p = malloc(n);
    if(!p){ perror("malloc"); exit(1); }
    memcpy(p, s, n);
    return p;
}

char* str_trim(char *s){
    if(!s) return s;
    char *a = s;
    while(*a && isspace((unsigned char)*a)) a++;
    char *b = a + strlen(a);
    while(b > a && isspace((unsigned char)b[-1])) --b;
    *b = '\0';
    return a;
}

Status status_from_str(const char *s){
    if(!s) return ST_INVALID;
    if(!strcasecmp(s,"Scheduled")) return ST_SCHEDULED;
    if(!strcasecmp(s,"Success"))   return ST_SUCCESS;
    if(!strcasecmp(s,"Failure"))   return ST_FAILURE;
    if(!strcasecmp(s,"Delayed"))   return ST_DELAYED;
    if(!strcasecmp(s,"Cancelled")) return ST_CANCELLED;
    return ST_INVALID;
}

const char* status_to_str(Status st){
    switch(st){
        case ST_SCHEDULED: return "Scheduled";
        case ST_SUCCESS:   return "Success";
        case ST_FAILURE:   return "Failure";
        case ST_DELAYED:   return "Delayed";
        case ST_CANCELLED: return "Cancelled";
        default:           return "Invalid";
    }
}

bool contains_case_insensitive(const char *hay, const char *needle){
    if(!hay || !needle) return false;
    size_t nlen = strlen(needle);
    if(nlen == 0) return true;
    for(const char *p = hay; *p; ++p){
        size_t i=0;
        while(i<nlen){
            int c1 = (unsigned char) p[i];
            int c2 = (unsigned char) needle[i];
            c1 = tolower(c1);
            c2 = tolower(c2);
            if(c2=='\0') break;
            if(c1!=c2) break;
            i++;
        }
        if(i==nlen) return true;
    }
    return false;
}
