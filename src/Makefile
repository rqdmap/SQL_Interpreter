all: main 
sql = list.cpp list.h sql.h sql.cpp rqdmap.h

main: lex.yy.c y.tab.c y.tab.h $(sql)
	g++ -o main lex.yy.c y.tab.c y.tab.h $(sql)

lex.yy.c: a.l
	lex a.l
y.tab.c y.tab.h: a.y
	yacc a.y -d
test: test.cpp $(sql)
	g++ -o test test.cpp $(sql)
