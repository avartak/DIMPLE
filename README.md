# DIMPLE

DIMPLE is an imperative programming language. It's my hobby project to learn about writing compilers and creating a real-world programming language. DIMPLE supports a procedural paradigm, with many C-like features like structs, unions, pointers, etc. It is possible to write, compile and execute programs (only some simple ones have been tested so far). DIMPLE makes use of the [LLVM infrastructure](https://llvm.org/) to first generate an intermediate representation of the source code and then to produce machine-dependent object code. 

You can read more about DIMPLE on the [wiki page](https://github.com/avartak/DIMPLE/wiki). The instructions to build the DIMPLE source code can be found [here](https://github.com/avartak/DIMPLE/wiki/Building-DIMPLE). The DIMPLE syntax is explained on [this page](https://github.com/avartak/DIMPLE/wiki/Syntax). To get a flavor of how DIMPLE source code looks like, take a look at the [Hello World program](https://github.com/avartak/DIMPLE/blob/master/test/helloworld/helloworld.avl). 

# Some Features

DIMPLE supports simple primitive data types (integers, floating point variables), pointers as well as certain compound types such as structs, unions and arrays. The DIMPLE syntax shares many similarities with C, but is different in some ways. 

## Representation

A unique feature of DIMPLE is a _representation_. A representation _stands for_ a type or a compile-time constant expression. For example,

```
C :: 2 + 3;
BinOp :: func(a : int32, b: int32) -> int32;
```

Here, ```C``` stands for the constant integer value 5, and ```BinOp``` stands for a _funtion type_ that takes two 32-bit integers as arguments and returns a 32-bit integer. The representation ```BinOp``` can be used as a stand-in replacement for all function types that have the signature ```func(int32, int32) -> int32```. For instance, one could define several functions that perform binary operations using the ```BinOp``` as the _signature_. These can be declared as follows

```
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

Representations can also be used for data-types. Representations can be used to alias or _typedef_ a given type. 

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
