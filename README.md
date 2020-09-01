**DISCLAIMER: We are aware the language is not finished. But we wanted to submit at _least_ something!**
# SeamLang (Seam)
## Foreword

It’s been a rough ride for many of us, but we would like to congratulate everyone who participated in the jam! :)

We personally had a lot of fun designing our language, learning about our tools, and exploring numerous ideas for how exactly we wanted our compiler to look - even if it meant our timing was a little off. It is no news to anyone that learning to use LLVM is a mammoth task, trust us - we know… but we don’t think this should discourage you! We highly recommend the following video to introduce yourself to LLVM’s IR and understand why we chose to use it: https://www.youtube.com/watch?v=m8G_S5LwlTo.

It has also been a pleasure to read all your wonderfully creative submissions, and we are very excited to see the winner of the jam.

We wish you all the best with your future endeavours! 

## Introduction

[Seam](https://github.com/rot13-password-hash/SeamLang) is a **compiled, statically typed, multi-paradigm programming language** designed primarily with **type-safety** and **ease-of-use** in mind. It is reminiscent of C++, Rust, and Go without being too on the nose (_we hope... but how unique can you really be in the programming language department_ :P). Preferably, we would like the language to be general purpose and we will try our best to make sure it is that way.

The design principles we had in mind for Seam whilst developing were:

1) **Easy to read** - it shouldn’t take developers a lot of effort to jump into a project and understand what the existing code is doing (within reason). This is a big problem in languages like Rust where a lot of the syntax can quickly lead to convoluted code.

2) **Type safety** - this one is self explanatory. If your code compiles, it shouldn’t crash. Duh. :eyes:

3) **Native thread support** (not yet implemented) - we believe that threading should be a language feature as opposed to an API. This would also go very well with our first principle, it should be very easy to use threads in a safe manner without driving yourself insane!

4) **Seamless (_pun intended_) modularity** - it should be super easy to modularise code, that means easy importing of functions in other files. 

## How does the compiler work?

We opted for a compiled language and chose to use LLVM as the backbone of our machine code generation, whilst writing our own lexer and parser.

The process for compilation is relatively simple:

1) Load in source file to Parser (which has a self-contained Lexer)
2) Parse lexemes into an abstract syntax tree
3) Run the AST through several passes
4) Run the AST through a code gen pass, generating a LLVM IR
5) Invoke LLVM object compilation and then linking

See! We told you it was easy! However, this is probably become increasingly more complex as we continue to improve the language.

## What is the language like?

Seam’s syntax should feel very familiar to those of you who’ve used C++, Rust, Go, or any other language with a similar syntax to those.

A Seam program consists of a collection of statements and expressions, with the top-most block any number of statements from a restricted set. Those being:

- Extern function declarations (will be removed in future)
- Function definitions 
- Type definitions

Extern function declarations are a temporary addition to allow us to use C functions while we write our own standard library.

### Function Defintions

Function definitions are blocks which accept any expressions and statements excluding any sort of function definition (however this will change in the future with the addition of lambda functions). 

Example: 

```
import io

fn entry() @constructor
{
    io.println("Hello World!")
}
```

### Type Definitions

The keyword `type` in Seam refers to an alias of an existing type, or a class definition as follows:

Example: 
```
import io

type int = i32

type Person
{
    name: string,
    age: int
}

fn add(lhs: int, rhs: int) -> int
{
    return lhs + rhs
}

fn main() @constructor
{
    me: Person = () // Implicit constructor
    me.name = "Chuck. E. Cheese."
    me.age = add(900, 99)
    
    io.println(me) // "{ name: "Chuck. E. Cheese", age: 900 }"
}
```

## So, how far did we get?

Well...this is embarrassing...we just missed the mark unfortunately shy of the deadline. Writing a whole programming language proved to be very difficult with such tight time constraints. Learning how to use LLVM took up a large portion of our time, but it was time well spent. We slaved away at the parser and code generation, but we did not have enough time to get anything of significant value up and running for you to test today :(

There were a few other hitches along the way related to busy schedules with existing projects, late start, University, and School. 

We're still commited to working on Seam though, so don't worry, we'll have something up soon! :P

### What do have implemented?

As of posting, we do not yet compile an executable: which means there's nothing for you to tinker around with. However a lot of concepts are fleshed out and parsed, with a few actually generating code. The following features are implemented:

- Statements and Expressions (parser is missing error recovery, coming soon:tm:)
- A rudimentary type system
- Basic control flow (if-elseif-else and while/for loops)
- Function calls

### What do have yet implement?

This one is a bit of a long one...so we're putting it in the **Future Aspirations** section.

## Code Samples
### FizzBuzz from 1 to 10
```
import io

fn entry() @constructor
{
    for (current_number -> 1, 10) // Loop from 1 - 10 inclusive
    {
        str := ""
        if (current_number % 3 == 0) { str += "Fizz" }
        if (current_number % 5 == 0) { str += "Buzz" }
        if (!(current_number % 3 && current_number % 5)) { str = current_number as string }
        
        io.println(str)
    }
}

```
### Recursive Fibonacci
```
import io

fn recursive_fib(n: i32) -> i32
{
    if (n <= 1)
    {
        return n
    }
    return recursive_fib(n - 1) + recursive_fib(n - 2)
}

fn entry() @constructor
{
    io.println(recursive_fib(5))
}
```

## Future aspirations
It is quite obvious that we have a long way to go before we have a language that is worthy of the being on the big stage, and we accept that. Not to compare Seam to Rome but, Rome wasn't built in a day, and we humble ourselves on this fact. So we've given ourselves two lists: in-progress and future.

In Progress (things we hope to have done soon):

- Generate executables (obviously, lol)
- Implement strings and arrays
- Refactor type system
- Function binding! (this is one of our fancy tricks)
- Smart printing of object values
- Formal language specification along with documentation
- Replace extern with internal standard library
- Write tests! :weary:

Future (things we hope to have, but aren't directly focused on at the moment):

- Garbage Collection (that isn't just reference counting)
- Implement aforementioned threading
- Language Server

Neither of these lists are finite or absolute, we are optimistic about the future of this language and we hope you are too! :)

## Do you have any ideas or want to help?

We're very happy to open new ideas and outside contributions, just let us know in the comments! :)

## Credits

@CoolDocterWho (he knows it's spelt wrong lol) - [GitHub](https://github.com/mathematicaldessert)

@JujharSinghKFX - [GitHub](https://github.com/jujhar16)

@MathiasPedersen - [GitHub](https://github.com/mathiaslpedersen)

---
