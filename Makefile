LambdaInterpreter: LambdaInterpreter.c LambdaReductionEngine.c LambdaReductionEngine.h LambdaParser.c LambdaParser.h MyUtils.c MyUtils.h LambdaSyntaxTree.h LambdaSyntaxTree.c LambdaUtils.c LambdaUtils.h InputReader.c InputReader.h termDictionary.h termDictionary.c dictionary.c dictionary.h
	gcc -Wall -g -o LambdaInterpreter LambdaInterpreter.c LambdaReductionEngine.c LambdaParser.c MyUtils.c LambdaUtils.c InputReader.c LambdaSyntaxTree.c termDictionary.c dictionary.c

tests: TEST_InputReader TEST_LambdaParser TEST_substitution

all: tests LambdaInterpreter

TEST_InputReader: TEST_InputReader.c InputReader.c InputReader.h MyUtils.c MyUtils.h
	gcc -Wall -g -o TEST_InputReader TEST_InputReader.c InputReader.c MyUtils.c

TEST_LambdaParser: TEST_LambdaParser.c LambdaParser.c LambdaParser.h MyUtils.c MyUtils.h LambdaSyntaxTree.h LambdaSyntaxTree.c LambdaUtils.c LambdaUtils.h InputReader.c InputReader.h
	gcc -Wall -g -o TEST_LambdaParser TEST_LambdaParser.c LambdaParser.c MyUtils.c LambdaUtils.c InputReader.c LambdaSyntaxTree.c

TEST_substitution: TEST_substitution.c LambdaReductionEngine.c LambdaReductionEngine.h LambdaParser.c LambdaParser.h MyUtils.c MyUtils.h LambdaSyntaxTree.h LambdaSyntaxTree.c LambdaUtils.c LambdaUtils.h InputReader.c InputReader.h
	gcc -Wall -g -o TEST_substitution TEST_substitution.c LambdaReductionEngine.c LambdaParser.c MyUtils.c LambdaUtils.c InputReader.c LambdaSyntaxTree.c

clean:
	rm -f TEST_InputReader TEST_LambdaParser TEST_substitution LambdaInterpreter
