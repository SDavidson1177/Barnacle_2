CXX = g++
CXXFLAGS = -std=c++14 -Wall -g -MMD
EXEC = ~/bin/bar
OBJECTS =  Expression.o FunctionDictionary.o LangFuncDictionary.o Parser.o StringDictionary.o Token.o VarDictionary.o Variable.o controlflow.o dictionary.o languageio.o main.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
.PHONY: clean
