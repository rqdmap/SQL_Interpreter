%{
#include "y.tab.h" 
%}

%option noyywrap
charType [a-zA-Z_]
digitType [0-9]
integer {digitType}+

id {charType}({charType}|{digitType})*
string \"({charType}|{digitType})*\"

int [iI][nN][tT]
double [dD][oO][uU][bB][lL][eE]
char [cC][hH][aA][rR]
create [cC][rR][eE][aA][tT][eE]
database [dD][aA][tT][aA][bB][aA][sS][eE]
databases [dD][aA][tT][aA][bB][aA][sS][eE][sS]
use [uU][sS][eE]
table [tT][aA][bB][lL][eE]
tables [tT][aA][bB][lL][eE][sS]
show [sS][hH][oO][wW]
desc [dD][eE][sS][cC]
insert [iI][nN][sS][eE][rR][tT]
into [iI][nN][tT][oO]
values [vV][aA][lL][uU][eE][sS]
select [sS][eE][lL][eE][cC][tT]
update [uU][pP][dD][aA][tT][eE]
delete [dD][eE][lL][eE][tT][eE]
from [fF][rR][oO][mM]
where [wW][hH][eE][rR][eE]
set [sS][eE][tT]
drop [dD][rR][oO][pP]
exit [eE][xX][iI][tT]

and [aA][nN][dD]|&&
or [oO][rR]|\|\|

%%
[\t \n]+ ;
{create} {return CREATE;}
{database} {return DATABASE;}
{databases} {return DATABASES;}
{use} {return USE;}
{table} {return TABLE;}
{tables} {return TABLES;}
{show} {return SHOW;}
{desc} {return DESC;}
{insert} {return INSERT;}
{into} {return INTO;}
{values} {return VALUES;}
{select} {return SELECT;}
{update} {return UPDATE;}
{delete} {return DELETE;}
{from} {return FROM;}
{where} {return WHERE;}
{set} {return SET;}
{drop} {return DROP;}
{exit} {return EXIT;}


{and} {return AND;}
{or} {return OR;}


{int} return INT;
{double} return DOUBLE;
{char} return CHAR;

{id} {
    yylval.str = strdup(yytext);
    return ID;
}
{integer} {
    yylval.str = strdup(yytext);
    return INTEGER;
}

{string} {
    // yylval.str = strdup(yytext + 1);
    // yylval.str[strlen(yylval.str) - 1] = 0;
    yylval.str = strdup(yytext);
    return STRING;
}

; {return FIN;}
\( {return LB;}
\) {return RB;}
, {return COMMA;}
\" {return QM;}
\< {return BELOW;}
= {return EQU;}
\* {return STAR;}

%%
// {digits}{optional_fraction}{optional_exponent} return NUMBER;
/* int main(){
    printf("词法分析成功,返回记号类别为%d\n", yylex());
    return 0;
} */