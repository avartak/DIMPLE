# DIMPLE

DIMPLE is a C-like imperative programming language. I started working on DIMPLE as a hobby project to learn about compilers and programming language design. The code base for the compiler frontend is relatively small (less than 10,000 lines). The compiler frontend makes use of the [LLVM infrastructure](https://llvm.org/) to first generate an intermediate representation (IR) of the source code, and then to produce machine-dependent object code. 

The DIMPLE syntax is explained in detail on this [wiki page](https://github.com/avartak/DIMPLE/wiki). You can find a quick overview of its features [below](https://github.com/avartak/DIMPLE/blob/main/README.md#feature-overview). Here are some example programs written in DIMPLE. You can find them in the [test](https://github.com/avartak/DIMPLE/tree/main/test) folder of the repository.

* [Simple factorial computing program](https://github.com/avartak/DIMPLE/blob/main/test/factorial/factorial.dmp)
* [Dynamic binding of functions using vtables](https://github.com/avartak/DIMPLE/blob/main/test/vtable/polymorph.dmp) 

# Installation

Interested in trying out DIMPLE? Here's what you need to do to build the compiler from its source code. 

## Prerequisites

* LLVM : Please lookup the [LLVM docs](https://llvm.org/docs/CMake.html) to figure out the best way to install the LLVM libraries (version 18.0 is recommended) on your machine. 

* Clang : You need a C++ compiler to build the DIMPLE source code. Clang is the current default choice. If you wish to switch to another compiler (e.g. GCC) you will need to modify the [Makefile](https://github.com/avartak/DIMPLE/blob/main/Makefile) accordingly, and make sure that the LLVM libraries are correctly linked. 

## Build DIMPLE

Clone the DIMPLE source code onto your machine with `git clone https://github.com/avartak/DIMPLE/`. You can build DIMPLE by running `make` in the top directory. You will need to adapt the following line in the Makefile according to your LLVM installation. 

```
LLVM     := /mnt/c/Users/adishvar/Work/Software/LLVM/Install
```

Assuming all prerequisites are correctly installed, `make` should produce an executable called `dimple`. You can test it by running the examples in the `test/helloworld` and `test/factorial` folders (simply go to these folders and run make). 

# Feature Overview

DIMPLE supports simple primitive data types (integers, floating point variables), pointers as well as certain compound types such as structs, unions and arrays. The DIMPLE syntax shares many similarities with C, but is different in some ways. 

## Location invariance of global constructs

A key distinction between DIMPLE and C is that the global constructs (i.e. global variables, functions and representations) can be used in a location invariant manner. In other words, DIMPLE does not require a global construct to be declared or defined before being used. This obviates the need for forward declarations. For example, the following is valid in DIMPLE. 

```
A :: struct (
    i : int64,
    p : @B
)

B :: struct (
    d : real64,
    p : @A
)
```

In fact, the only reason to use declarations in DIMPLE is to link externally defined code (e.g. preexisting libraries). 

## Representations

A unique feature of DIMPLE is a _representation_. A representation _stands for_ a type or a compile-time constant expression. For example,

```
C :: (2 + 3) | (1 << 4);
BinOp :: func(a : int32, b: int32) -> int32;
```

Here, ```C``` stands for the constant integer value 21, and ```BinOp``` stands for a _funtion type_ that takes two 32-bit integers as arguments and returns a 32-bit integer. The representation ```BinOp``` can be used as a stand-in replacement for all function types that have the _signature_ ```func(int32, int32) -> int32```. For instance, consider the following functions that perform operations on a pair of integers. 

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
P :: @P;
```

Or one could construct an array whose element type is it's own pointer

```
A :: [4]@A;
```

One could also construct a function that takes as an argument (or returns) a pointer to it's own type

```
F :: func(fptr : @F);
```

## Anonymous members 

AVL provides two ways to access members of structs and unions. The members may have names, and these named members may be accessed through the ```.``` operator as in C. Alternatively, any member of a struct or a union may be accessed using the index operator 

```
S :: struct (
    int32,
    d : real64
)

s := S{};
s[0] = 4;
s.d = 3.14;
s[1] = 2.5;
```

## Function arguments
DIMPLE allows both pass-by-value and pass-by-reference arguments to functions. By default, arguments are passed by value. However, if the argument name is preceded by the `$` symbol in the function signature, that argument is passed by reference. 

```
flip := func($b : bool) {

    if b {
        b = false;
    }
    else {
        b = true;
    }
}
```

## Aliases or references

DIMPLE allows the use of _aliases_ or references. An alias is basically another name for a given memory instance. This instance can be a variable, a function, or even a member of a compound variable. In case of variables, modifying the content of an alias will also change the content of the corresponding instance.  

```
a := [4]int8;
$i := a[2];
p := int32{0};
$q := p;
```


# Code Arrangement

For those interested in browsing through the compiler code, here is a quick overview of how the source code is arranged. 
* At the top level, the compiler code base has 3 folders.
  * The [include](https://github.com/avartak/DIMPLE/tree/main/include) folder contains all the C++ header files
  * The [src](https://github.com/avartak/DIMPLE/tree/main/include) folder contains the corresponding C++ source files
* Both the [include](https://github.com/avartak/DIMPLE/tree/main/include) and [src](https://github.com/avartak/DIMPLE/tree/main/include) folders have the same sub-directory structure. 
  * The [Start](https://github.com/avartak/DIMPLE/tree/main/src/Start) folder contains the top-level code elements of the compiler. This includes the [`main`](https://github.com/avartak/DIMPLE/blob/main/src/Start/Main.cc) function and the [`compile`](https://github.com/avartak/DIMPLE/blob/main/src/Start/Compile.cc) function.
  * The [Common](https://github.com/avartak/DIMPLE/tree/main/src/Common) folder contains common elements that are used by various components of the compiler. This includes certain LLVM [global variables](https://github.com/avartak/DIMPLE/blob/main/include/Common/Globals.h), and the definition of a few important base classes : the [`Pass`](https://github.com/avartak/DIMPLE/blob/main/include/Common/Pass.h) class is the base class of the 3 passes that are run by the compiler ([`Parser`](https://github.com/avartak/DIMPLE/blob/main/include/Parser/Parser.h), [`Translator`](https://github.com/avartak/DIMPLE/blob/main/include/Translator/Translator.h), [`Backend`](https://github.com/avartak/DIMPLE/blob/main/include/Backend/Backend.h)), the [`TypeBase`](https://github.com/avartak/DIMPLE/blob/main/include/Common/TypeBase.h) class is the base class of all the types used in DIMPLE, and the [`Error`](https://github.com/avartak/DIMPLE/blob/main/include/Common/Error.h) class is used extensively is all the passes for error reporting.   
  * The [test](https://github.com/avartak/DIMPLE/tree/main/test) folder contains some test programs to demonstrate how dimple source code looks like. In the future, this area will hopefully be populated by an extensive set of unit tests.
  * The [AST](https://github.com/avartak/DIMPLE/tree/main/include/AST) folder contains classes representing various nodes of the _Abstract Syntax Tree_ or AST that is generated by the compiler during the _parsing_ step
  * The [IR](https://github.com/avartak/DIMPLE/tree/main/include/IR) folder contains various classes that encapsulate the _translation_ of the AST to some intermediate representation (IR). The DIMPLE compiler uses [LLVM IR](https://llvm.org/docs/LangRef.html) but the interface provided by the classes defined in the [IR](https://github.com/avartak/DIMPLE/tree/main/include/IR) folder is quite generic.
  * The [IO](https://github.com/avartak/DIMPLE/tree/main/include/IO) folder contains classes that manage file inputs (source files, includes)
  * The [Lexer](https://github.com/avartak/DIMPLE/tree/main/include/Lexer) folder contains the code of the lexical analyzer or tokenizer
  * The [Parser](https://github.com/avartak/DIMPLE/tree/main/include/Parser) folder contains code that performs syntax analysis of the source code and create the AST.
  * The [Translator](https://github.com/avartak/DIMPLE/tree/main/include/Translator) folder contains code that translates the AST to the IR. 
  * The [Backend](https://github.com/avartak/DIMPLE/tree/main/include/Backend) folder contains code that translates the IR to machine code. For now, this is a relatively straightforward plug into LLVM's backend functionalities. 

# Contributing

You are welcome to contribute to the development of DIMPLE. You can help in several ways
* Report bugs in the code (and possible fixes)
* Suggest implementation improvements if you feel some code can be written in a better way
* Work on implementing new features to extend the language

If you want to contribute changes to the source code, please submit a pull request. 
