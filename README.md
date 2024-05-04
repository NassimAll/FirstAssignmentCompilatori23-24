# First assignment compilers class

Implementing the following local optimizations to my llvm pass:

1. Algebraic Identity: Simplifying basic equations:
    - `x + 0 = 0 + x = x -> x`
    - `x * 1 = 1 * x = x -> x`

2. Strength Reduction: Transforming an expensive operation into a simpler one:
    - `15 * x = x * 15 -> (x << 4) - x`
    - `y = x / 8 -> y = x >> 3`

3. Multi-Instruction Optimization:
    - `a = b + 1, c = a - 1 -> a = b + 1, c = b`
