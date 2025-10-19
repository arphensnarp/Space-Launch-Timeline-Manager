#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "sltm.h"

static bool is_leap(int y){
    return (y%4==0 && y%100!=0) || (y%400==0);
}

static int mdays(int y, int m){
    static const int d[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if(m==2) return d[1] + (is_leap(y)?1:0);
    if(m>=1 && m<=12) return d[m-1];
    return 0;
}

bool date_parse_ymd(const char *s, Date *out){
    if(!s || strlen(s)!=10) return false;
    if(!(isdigit(s[0])&&isdigit(s[1])&&isdigit(s[2])&&isdigit(s[3])&&
         s[4]=='-'&&isdigit(s[5])&&isdigit(s[6])&&
         s[7]=='-'&&isdigit(s[8])&&isdigit(s[9]))) return false;

    int y = (s[0]-'0')*1000 + (s[1]-'0')*100 + (s[2]-'0')*10 + (s[3]-'0');
    int m = (s[5]-'0')*10 + (s[6]-'0');
    int d = (s[8]-'0')*10 + (s[9]-'0');
    if(m<1||m>12) return false;
    int md = mdays(y,m);
    if(d<1||d>md) return false;

    out->year=y; out->month=m; out->day=d;
    return true;
}

int date_cmp(Date a, Date b){
    if(a.year!=b.year)   return (a.year<b.year)?-1:1;
    if(a.month!=b.month) return (a.month<b.month)?-1:1;
    if(a.day!=b.day)     return (a.day<b.day)?-1:1;
    return 0;
}

bool date_in_range(Date x, Date a, Date b){
    return date_cmp(a,x)<=0 && date_cmp(x,b)<=0;
}

