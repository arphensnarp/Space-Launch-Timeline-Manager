#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "sltm.h"

static volatile sig_atomic_t running = 1;
static void on_sigint(int s){ (void)s; running = 0; }

static void cmd_help(void){
    puts("Commands:");
    puts("  load <file>");
    puts("  add <id>,<YYYY-MM-DD>,<vehicle>,<mission>,<site>,<status>");
    puts("  update <id> field=value ...   (fields: date,vehicle,mission,site,status)");
    puts("  delete <id>");
    puts("  range <YYYY-MM-DD> <YYYY-MM-DD>");
    puts("  find <keyword>");
    puts("  export <file>");
    puts("  help");
}

static void print_event(const Event *e){
    printf("%s,%04d-%02d-%02d,%s,%s,%s,%s\n",
        e->id,e->date.year,e->date.month,e->date.day,
        e->vehicle,e->mission,e->site,status_to_str(e->status));
}

int main(void){
    signal(SIGINT, on_sigint);

    Node *head = NULL;
    char line[8192];

    while(running){
        printf("sltm> "); fflush(stdout);
        if(!fgets(line,sizeof(line),stdin)) break;
        // trim newline
        size_t n=strlen(line);
        while(n>0 && (line[n-1]=='\n'||line[n-1]=='\r')) line[--n]='\0';

        char *cmd = strtok(line," ");
        if(!cmd) continue;

        if(!strcmp(cmd,"help")) { cmd_help(); continue; }

        if(!strcmp(cmd,"load")){
            char *f = strtok(NULL,"");
            if(!f){ puts("ERR: usage: load <file>"); continue; }
            int ignored=0, loaded = csv_load_file(f, &head, &ignored);
            if(loaded>=0) printf("Loaded %d; ignored %d\n", loaded, ignored);
            continue;
        }

        if(!strcmp(cmd,"export")){
            char *f = strtok(NULL,"");
            if(!f){ puts("ERR: usage: export <file>"); continue; }
            if(csv_export_file(f, head)) puts("Exported."); else puts("Export failed.");
            continue;
        }

        if(!strcmp(cmd,"add")){
            char *rec = strtok(NULL,"");
            if(!rec){ puts("ERR: usage: add <csv-record>"); continue; }
            char buf[8192]; strncpy(buf, rec, sizeof(buf)); buf[sizeof(buf)-1]='\0';
            char *id,*date,*veh,*mis,*site,*stat;
            // reuse csv split helper logic inline
            int commas=0; for(char *p=buf; *p; ++p) if(*p==',') commas++;
            if(commas<5){ puts("ERR: need 6 CSV fields"); continue; }
            // simple split:
            char *f[6]; char *p=buf; for(int i=0;i<6;i++){ f[i]=p; char *c=strchr(p, i<5? ',' : '\0'); if(i<5){ *c='\0'; p=c+1; } }
            id=str_trim(f[0]); date=str_trim(f[1]); veh=str_trim(f[2]); mis=str_trim(f[3]); site=str_trim(f[4]); stat=str_trim(f[5]);
            Date d; if(!date_parse_ymd(date,&d)){ puts("ERR: invalid date"); continue; }
            Status st = status_from_str(stat); if(st==ST_INVALID){ puts("ERR: invalid status"); continue; }
            Event e = { xstrdup(id), d, xstrdup(veh), xstrdup(mis), xstrdup(site), st };
            head = list_insert_sorted(head, e);
            puts("OK added.");
            continue;
        }

        if(!strcmp(cmd,"delete")){
            char *id = strtok(NULL," \t");
            if(!id){ puts("ERR: usage: delete <id>"); continue; }
            if(list_delete_by_id(&head,id)) puts("OK deleted."); else puts("Not found.");
            continue;
        }

        if(!strcmp(cmd,"update")){
            char *id = strtok(NULL," \t");
            if(!id){ puts("ERR: usage: update <id> <field=value> ..."); continue; }
            Node *node = list_find_by_id(head,id);
            if(!node){ puts("Not found."); continue; }
            char *kv;
            while((kv=strtok(NULL," "))){
                char *eq = strchr(kv,'=');
                if(!eq){ printf("Skip invalid token: %s\n",kv); continue; }
                *eq='\0'; char *key=kv; char *val=eq+1;
                if(!strcmp(key,"date")){
                    Date d; if(date_parse_ymd(val,&d)) node->e.date=d; else puts("  invalid date (kept old).");
                } else if(!strcmp(key,"vehicle")){
                    free(node->e.vehicle); node->e.vehicle = xstrdup(val);
                } else if(!strcmp(key,"mission")){
                    free(node->e.mission); node->e.mission = xstrdup(val);
                } else if(!strcmp(key,"site")){
                    free(node->e.site); node->e.site = xstrdup(val);
                } else if(!strcmp(key,"status")){
                    Status st = status_from_str(val); if(st!=ST_INVALID) node->e.status=st; else puts("  invalid status (kept old).");
                } else {
                    printf("  unknown field: %s\n", key);
                }
            }
            puts("OK updated.");
            continue;
        }

        if(!strcmp(cmd,"range")){
            char *a = strtok(NULL," \t");
            char *b = strtok(NULL," \t");
            if(!a||!b){ puts("ERR: usage: range <YYYY-MM-DD> <YYYY-MM-DD>"); continue; }
            Date da,db;
            if(!date_parse_ymd(a,&da) || !date_parse_ymd(b,&db)){ puts("ERR: invalid date"); continue; }
            list_print_range(head, da, db);
            continue;
        }

        if(!strcmp(cmd,"find")){
            char *kw = strtok(NULL,"");
            if(!kw){ puts("ERR: usage: find <keyword>"); continue; }
            kw = str_trim(kw);
            for(Node *p=head; p; p=p->next){
                if(contains_case_insensitive(p->e.vehicle, kw) ||
                   contains_case_insensitive(p->e.mission, kw)){
                    print_event(&p->e);
                }
            }
            continue;
        }

        if(!strcmp(cmd,"quit") || !strcmp(cmd,"exit")) break;

        puts("Unknown command. Type 'help'.");
    }

    list_free(head);
    return 0;
}
