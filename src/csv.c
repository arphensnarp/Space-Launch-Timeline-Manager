#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sltm.h"

static bool split6(char *line, char **f1, char **f2, char **f3, char **f4, char **f5, char **f6){
    // Simple CSV: 6 fields separated by ','; no embedded commas/quotes.
    char *p = line;
    char *fields[6] = {0};
    for(int i=0;i<6;i++){
        fields[i] = p;
        char *c = strchr(p, i<5 ? ',' : '\0');
        if(i<5){
            if(!c) return false;
            *c = '\0';
            p = c+1;
        }
    }
    *f1=fields[0]; *f2=fields[1]; *f3=fields[2]; *f4=fields[3]; *f5=fields[4]; *f6=fields[5];
    return true;
}

int csv_load_file(const char *path, Node **phead, int *ignored){
    FILE *fp = fopen(path,"r");
    if(!fp){ perror(path); return -1; }
    char buf[4096];
    int line=0, bad=0, good=0;

    if(!fgets(buf,sizeof(buf),fp)){ 
        fclose(fp); 
        return 0; 
    } // header

    while(fgets(buf,sizeof(buf),fp)){
        line++;
        // strip newline
        size_t n=strlen(buf);
        while(n>0 && (buf[n-1]=='\n'||buf[n-1]=='\r')) buf[--n]='\0';

        char *l = str_trim(buf);
        if(!*l) { bad++; continue; }

        char *id,*date,*veh,*mis,*site,*stat;
        if(!split6(l,&id,&date,&veh,&mis,&site,&stat)){ bad++; continue; }

        id=str_trim(id); date=str_trim(date); veh=str_trim(veh);
        mis=str_trim(mis); site=str_trim(site); stat=str_trim(stat);

        if(!*id||!*date||!*veh||!*mis||!*site||!*stat){ bad++; continue; }

        Date d;
        if(!date_parse_ymd(date,&d)){ bad++; continue; }

        Status st = status_from_str(stat);
        if(st==ST_INVALID){ bad++; continue; }

        Event e = { xstrdup(id), d, xstrdup(veh), xstrdup(mis), xstrdup(site), st };
        *phead = list_insert_sorted(*phead, e);
        good++;
    }
    
    fclose(fp);
    if(ignored) *ignored = bad;
    return good;
}

bool csv_export_file(const char *path, Node *head){
    FILE *fp = fopen(path,"w");
    if(!fp){ perror(path); return false; }
    fprintf(fp, "id,date,vehicle,mission,site,status\n");
    for(Node *p=head; p; p=p->next){
        fprintf(fp,"%s,%04d-%02d-%02d,%s,%s,%s,%s\n",
            p->e.id,p->e.date.year,p->e.date.month,p->e.date.day,
            p->e.vehicle,p->e.mission,p->e.site,status_to_str(p->e.status));
    }
    fclose(fp);
    return true;
}
