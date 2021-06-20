/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CREATE = 258,
    DATABASE = 259,
    DATABASES = 260,
    USE = 261,
    TABLE = 262,
    TABLES = 263,
    SHOW = 264,
    DESC = 265,
    INSERT = 266,
    INTO = 267,
    VALUES = 268,
    SELECT = 269,
    UPDATE = 270,
    DELETE = 271,
    DROP = 272,
    EXIT = 273,
    INT = 274,
    DOUBLE = 275,
    CHAR = 276,
    AND = 277,
    OR = 278,
    FROM = 279,
    WHERE = 280,
    SET = 281,
    FIN = 282,
    LB = 283,
    RB = 284,
    COMMA = 285,
    BELOW = 286,
    EQU = 287,
    STAR = 288,
    QM = 289,
    ID = 290,
    STRING = 291,
    INTEGER = 292
  };
#endif
/* Tokens.  */
#define CREATE 258
#define DATABASE 259
#define DATABASES 260
#define USE 261
#define TABLE 262
#define TABLES 263
#define SHOW 264
#define DESC 265
#define INSERT 266
#define INTO 267
#define VALUES 268
#define SELECT 269
#define UPDATE 270
#define DELETE 271
#define DROP 272
#define EXIT 273
#define INT 274
#define DOUBLE 275
#define CHAR 276
#define AND 277
#define OR 278
#define FROM 279
#define WHERE 280
#define SET 281
#define FIN 282
#define LB 283
#define RB 284
#define COMMA 285
#define BELOW 286
#define EQU 287
#define STAR 288
#define QM 289
#define ID 290
#define STRING 291
#define INTEGER 292

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 27 "a.y"

    char *str;
    int num;
    struct {
        char *p;
        int bytes;   
    } M;

#line 140 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
