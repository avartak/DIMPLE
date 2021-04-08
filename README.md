# DIMPLE

DIMPLE is an imperative programming language. It's my hobby project to learn about writing compilers and creating a real-world programming language. DIMPLE supports a procedural paradigm, with many C-like features like structs, unions, pointers, etc. It is possible to write, compile and execute programs (only some simple ones have been tested so far). DIMPLE makes use of the [LLVM infrastructure](https://llvm.org/) to first generate an intermediate representation of the source code and then to produce machine-dependent object code. 

To get a flavor of how DIMPLE source code looks like, take a look at the [Hello World program](https://github.com/avartak/DIMPLE/blob/master/test/helloworld/helloworld.dmp) and the [Factorial program](https://github.com/avartak/DIMPLE/blob/main/test/factorial/factorial.dmp). You can read more about DIMPLE on the [wiki page](https://github.com/avartak/DIMPLE/wiki). The instructions to build the DIMPLE source code can be found [here](https://github.com/avartak/DIMPLE/wiki/Building-DIMPLE). The DIMPLE syntax is explained on [this page](https://github.com/avartak/DIMPLE/wiki/Syntax). In particular, the lexis of the language (all possible lexemes that can be constructed) is described [here](https://github.com/avartak/DIMPLE/wiki/DIMPLE-Syntax#lexis), and the grammar of the language is detailed [here](https://github.com/avartak/DIMPLE/wiki/DIMPLE-Syntax#dimple-grammar)

# Some Features

DIMPLE supports simple primitive data types (integers, floating point variables), pointers as well as certain compound types such as structs, unions and arrays. The DIMPLE syntax shares many similarities with C, but is different in some ways. 

## Representation

A unique feature of DIMPLE is a _representation_. A representation _stands for_ a type or a compile-time constant expression. For example,

```
C :: 2 + 3;
BinOp :: func(a : int32, b: int32) -> int32;
```

Here, ```C``` stands for the constant integer value 5, and ```BinOp``` stands for a _funtion type_ that takes two 32-bit integers as arguments and returns a 32-bit integer. The representation ```BinOp``` can be used as a stand-in replacement for all function types that have the _signature_ ```func(int32, int32) -> int32```. For instance, consider the following functions that perform operations on a pair of integers. 

```
add := func(a : int32, b: int32) {
    return a+b;
}
subtract := func(a : int32, b: int32) {
    return a-b;
}
multiply := func(a : int32, b: int32) {
    return a*b;
}
```

These can be written as

```
BinOp :: func(a : int32, b: int32) -> int32;
add := BinOp {
    return a+b;
}
subtract := BinOp {
    return a-b;
}
multiply := BinOp {
    return a*b;
}
```

Representations can also be used for data-types. A representation serves as an _alias_ of a given type. 

```
char :: int8
```

A representation is the only way to define a _named_ ```struct``` or a ```union```. 

```
S :: struct (
    i : int32,
    f : real32
)
```

There are some interesting and unique ways of using representations, which don't have natural counterparts in C. For example, one may construct a self-referencing pointer type

```
P :: %P
```

Or one could construct an array whose element type is it's own pointer

```
A :: [4]%A
```

One could also construct a function that takes as an argument (or returns) a pointer to it's own type

```
F :: func(%F)
```

## Anonymous members 

AVL provides two ways to access members of structs and unions. The members may have names, and these named members may be accessed through the ```.``` operator as in C. Alternatively, any member of a struct or a union may be accessed using the index operator 

```
S :: struct (
    int32
    d : real64
)

s := S{};
s[0] = 4;
s.d = 3.14;
s[1] = 2.5;
```

## Function arguments
DIMPLE allows both pass-by-value and pass-by-reference arguments to functions. By default, arguments are passed by value. However, if the argument name is preceded by the `@` symbol in the function signature, that argument is passed by reference. 

```
flip := func(@b : bool) {

    if b {
        b = false;
    }
    else {
        b = true;
    }
}
```

Note, that unlike C++ DIMPLE does not support any explicit use of references (or aliases). 

# Code Arrangement

For those interested in browsing through the compiler code, here is a quick overview of how the source code is arranged. 
* The [start](https://github.com/avartak/DIMPLE/tree/main/start) folder contains the top-level code elements of the compiler. This includes the [`main`](https://github.com/avartak/DIMPLE/blob/main/start/src/Main.cc) function, the [`compile`](https://github.com/avartak/DIMPLE/blob/main/start/src/Compile.cc), definition of certain key LLVM related [global variables](https://github.com/avartak/DIMPLE/blob/main/start/src/Globals.cc), and the definition of a few important base classes : the [`Pass`](https://github.com/avartak/DIMPLE/blob/main/start/include/Pass.h) class is the base class of the 3 passes that are run by the compiler ([`Parser`](https://github.com/avartak/DIMPLE/blob/main/parser/include/Parser.h), [`Translator`](https://github.com/avartak/DIMPLE/blob/main/translator/include/Translator.h), [`Backend`](https://github.com/avartak/DIMPLE/blob/main/backend/include/Backend.h)), the [`TypeBase`](https://github.com/avartak/DIMPLE/blob/main/start/include/TypeBase.h) class is the base class of all the types used in DIMPLE, and the [`Error`](https://github.com/avartak/DIMPLE/blob/main/start/include/Error.h) is used extensively is all the passes for error reporting.   
* The [test](https://github.com/avartak/DIMPLE/tree/main/test) folder contains a couple of test programs to demonstrate how dimple source code looks like. In the future, this area will hopefully be populated by an extensive set of unit tests.
* The [ast](https://github.com/avartak/DIMPLE/tree/main/ast) folder contains classes representing various nodes of the _Abstract Syntax Tree_ or AST that is generated by the compiler during the _parsing_ step
* The [IR](https://github.com/avartak/DIMPLE/tree/main/IR) folder contains various classes that encapsulate the _translation_ of the AST to some intermediate representation (IR). The DIMPLE compiler uses [LLVM IR](https://llvm.org/docs/LangRef.html) but the interface provided by the classes defined in the [IR](https://github.com/avartak/DIMPLE/tree/main/IR) folder is quite generic.
* The [input](https://github.com/avartak/DIMPLE/tree/main/input) folder contains classes that manage file inputs (source files, includes)
* The [lexis](https://github.com/avartak/DIMPLE/tree/main/lexis) folder contains the code of the lexical analyzer or tokenizer
* The [parser](https://github.com/avartak/DIMPLE/tree/main/parser) folder contains code that performs syntax analysis of the source code and create the AST.
* The [translator](https://github.com/avartak/DIMPLE/tree/main/translator) folder contains code that translates the AST to the IR. 
* The [backend](https://github.com/avartak/DIMPLE/tree/main/backend) folder contains code that translates the IR to machine code. For now, this is a relatively straightforward plug into LLVM's backend functionalities. 
