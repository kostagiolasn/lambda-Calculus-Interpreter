---
GENERATOR: 'Mozilla/4.76 \[en\] (X11; U; Linux 2.4.2-2 i686) \[Netscape\]'
title: 'Lambda Calculus Interpreter - Nikos - Stefanos Kostagiolas'
...

Lambda Calculus Interpreter
===========================

### Nikos - Stefanos Kostagiolas - <sdi1100039@di.uoa.gr>

    This is a Lambda Calculus Interpreter implemented in ANSI C for the
requirements of the course "Programming Languages Principles".\
 

### Quickstart

    If you want to build the Interpreter, just type "make".\
 

### Syntax

    The syntax used is the same syntax that Haskell uses to represent
Lambda-terms. An extension was added for ease of use, so you can input
lambda-terms using the 'dot' (.) instead of the 'arrow' (-&gt;). Please
note that all lambda-terms can either be fully parenthesized or not.

'**Extended BNF**' syntax for lambda-terms:

    <term> ::=     <var>                   |
                   <number>                |
                   <oper>                  |
                   (<term> <term>)         |
                   (\<var> -> <term>)      |
                   (\<var> . <term>)

    <var> ::=      <letter>[<letter>|<digit>]*

    <number> ::=   [<digit>]+ | -[<digit>]+

    <oper> ::=     +  |  -  |  *  |  /  |  %

    <digit> ::=    0 | 1 | 2 | ..... | 8 | 9

    <letter> ::=   A | B | C | ..... | Y | Z | a | b | ..... | y | z

(Reminder:    \[\]\* means zero or more occurences of\
        \[\]+ means one or more occurences of    )\
 

**WARNING:**  Be sure not to use variables in the form of

        s[<digit>]+

    as these variable names are used to insert new unbounded variables.\
 

**NOTES on Syntax:**

-   This interpreter works fine with fully parenthesized input.
    Simplified input also works but is not thoroughly checked
-   Some of the spaces in the syntax can be omitted, if the result of
    such an operation is the same term. All of the spaces, are
    not limiting. You can use as many as you want (spaces or tabs).
-   Each variable may have as many characters as it takes. The
    interpreter has not any constriction according to
    variable-character limits.
-   In a line, anything following a '\#' character is considered a
    comment and thus is ignored and removed by the input preprocessor.
    Also, a blank or empty line will be ignored.

### 

### Command-line Parameters

    You can specify an arbitrary amount of command-line parameters.
These will be treated as filenames and their input will be read in the
order that they where specified.\
    So, if you want to include the default prelude provided, you must
start the interpreter as 'LambdaInterpreter prelude1'. (you can also
":read prelude1" when you are in the interpreter to)\
 \
 

### Interpreter Commands

    Any input to t&lt;&gt;he interpreter will be treated as a
lambda-term to be reduced, unless the first non-blank character is a
colon ':'. This signifies the start of a interpreter command.\
    For more info on the commands, inside the interpreter type ':help'.
The following text will appear:\
 

+--------------------------------------------------------------------------+
|     Lambda Interpreter  -  Nikos-Stefanos Kostagiolas, sdi1100039@di.uoa |
| .gr                                                                      |
|                                                                          |
|      Commands        Description                                         |
|     :read <file>    Read input from filename <file>                      |
|     :prelude        Show the currently binded variables                  |
|     :details        Print details for every normalization (redexes, term |
| s, etc)                                                                  |
|     :nodetails      Don't print details for normalizations.              |
|     :limit <num>    Perform a maximum of <num> reductions for every norm |
| alization                                                                |
|     :nolimit        No limit in number of reductions (same with ":limit  |
| 0")                                                                      |
|     :let <i> <t>    Bind variable <i> with lambda-term <t>. Every follow |
| ing                                                                      |
|                       occurence of  <i> will be replaced by lambda-term  |
| <t>                                                                      |
|     :unlet <var>    Unbind variable <var>                                |
|     :unletall       Unbind all variables currently binded                |
|     :reduce <var>   Normalize binded variable <var>                      |
|     :dot            Use "dot" notation for output, example:  (\x.x)      |
|     :arrow          Use "arrow" notation for output, example: (\x -> x)  |
|     :exit           Exit Interpreter, if in input from file then close f |
| ile                                                                      |
|     :help           Show this help message                               |
|     :settings       Print all the interpreter setting (dot,details,limit |
| ,prompt)                                                                 |
|     :prompt         Enable prompt                                        |
|     :noprompt       Disable prompt                                       |
|                                                                          |
|     lambda>                                                              |
+--------------------------------------------------------------------------+

### 

### Default Prelude - prelude1

    # A simple prelude for the Lambda Calculus Interpreter
    # Nikos-Stefanos Kostagiolas
    # AM: 1115201100039


    # booleans
    :let true (\x -> (\y -> x))
    :let false (\x -> (\y -> y))
    :let cond (\a -> (\b -> (\c -> ((a b) c))))

    # boolean operators
    :let not (\x -> (((cond x) false) true))
    :let and (\x -> (\y -> ((x y) false)))
    :let or (\x -> (\y -> ((x true) y)))

    # pairs
    :let pair (\x -> (\y -> (\z -> ((z x) y))))
    :let fst (\n -> (n true))
    :let snd (\n -> (n false))

    # The twice function
    #:let twice (\f -> (\x -> (f (f x))))

    # Church Numerals Base
    :let zero     (\f -> (\z -> z))
    :let succ     (\n -> (\f -> (\x -> ((n f) (f x)))))

    # define some numerals to play with
    :let one      (succ zero)
    :let two      (succ one)
    :let three    (succ two)
    :let four     (succ three)
    :let five     (succ four)

    # Numerical Operations on Church Numerals
    :let add      (\n -> (\m -> (\f -> (\x -> ((n f) ((m f) x))))))
    :let mul      (\n -> (\m -> (\f -> (n (m f)))))
    :let exp      (\n -> (\m -> (m n)))

    # general numeral utility bindings
    :let unchurch (\n -> ((n (\x.((+ 1) x))) 0))
    :let iszero   (\n -> ((n (\x -> false)) true))
    :let zeropair ((pair zero) zero)
    :let pred     (\x -> (\y -> (\z -> (((x (\p -> (\q -> (q  (p y))))) (\y -> z)) (\x -> x)))))

    #A non-terminating expression
    :let nonterminate  ((\x -> (x x)) (\x -> (x x)))

    # A fixpoint combinator
    :let fix      (\f -> ((\x -> (f (x x)))(\x -> (f (x x)))))

    # Example of using fixpoint to define recursive functions, e.g factorial
    :let factorial (fix (\f -> (\n -> (((cond (iszero n)) one) ((mul n) (f (pred n)))))))

    # Example of simplified terms
    1. ((\x->x)y)(\x->z) becomes (y (\x->z))
    2. (\x->\y->z)(\x->x) becomes (\y->z)
    3. \x->(\y->y)(\z->x) becomes (\x->(\z->x))

### Bugs or Problems

    The only disadvantage is that there is minimum checking if more
memory can be obtained. No other problems were discovered during the
tesing and building.\
 

### Lambda-Terms used for Testing

    The "EXAMPLES" file contains some testing input for the interpreter.
Please note that these examples might take a while to finish (5\^5
performs around 20.000 reductions). These are the contents of the file:

    #calculate 5*5*5*5*5, and show readable result, which should be 3125
    (unchurch ((exp five) five))

    #calculate 5! and show readable result, which should be 120
    (unchurch (factorial five))

    #arithmetic example:
    # performs:    test(x, y) = 11 + ( ((x*17 + 32*y) / 3) - 2 )
    # for parameters 4 and 5, which should give 85
    (((\x.(\y.((+11 )((-((/((+((*x )17 ))((*32 )y )))3 ))2 )))) 4) 5)

 

### Implementation Design Issues

-   The "substitution" procedure was implemented, so as to easily
    perform beta reductions. Also, "substitution" was used when
    replacing binded variables.
-   As a result of the above, alpha reductions where not needed, so they
    were not implemented.
-   Numbers integration was tricky. Numbers are represented as variables
    and are distinguished by them using the syntax convension that
    variables cannot start with a digit (or a minus sign).
-   Negative numbers are supported. You can see it from the syntax.
-   A CALC-reduction type was added which computes results of built-in
    arithmetics. The CALC-redex is:
-   A lambda-term wrapper around a generic dictionary was created, so as
    to handle the bindings. The generic dictionary code (dictionary.c)
    was created during the Operating Systems course.
-   The interpreter has no memory leaks. After performing 1.000.000 of
    reductions, the process size never increased.
-   The development was done in Ubuntu 14.04 platform, where no errors
    were detected. The source code was written in ANSI C.

------------------------------------------------------------------------

+--------------------------------------+--------------------------------------+
| Nikos-Stefanos Kostagiolas -         | <div align="right">                  |
| <sdi1100039@di.uoa.gr>\              |                                      |
| AM: 1115201100039                    | February 22, 2015                    |
|                                      |                                      |
|                                      | </div>                               |
+--------------------------------------+--------------------------------------+
