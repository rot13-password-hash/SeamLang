# Seam
Seam is statically typed, multi-paradigm programming language designed with type-safety and ease-of-use in mind.

# Example Code
## For Loop
```rust
fn entry() @constructor
{
    for (current_number -> 0, 10) // Loop from 0 - 10 inclusive
    {
        str := ""
        if (current_number % 3 == 0) { str += "Fizz" }
        if (current_number % 5 == 0) { str += "Buzz" }
        if (current_number % 3 && current_number % 5) { str = current_number as string }
        
        io.println(str)
    }
}
```
## Recursive Fibonacci
```rust
// Example Recursive Fibonacci
fn recursive_fibonacci(n: i32) -> i32
{
    if (n <= 1)
    {
        return n
    }
    return recursive_fibonacci(n - 1) + recursive_fib(n - 2)
}

fn entry() @constructor
{
    io.println(recursive_fib(5))
}
```

## Credits
We're very thankful to all our contributors who've helped make this project what it is!

### Original Maintainers
Pelanyo Kamara - [GitHub](https://github.com/mathematicaldessert) - [Twitter](https://twitter.com/pkthedev)

Jujhar Singh - [GitHub](https://github.com/jujhar16)

Mathias Pedersen - [GitHub](https://github.com/mathiaslpedersen)

---
