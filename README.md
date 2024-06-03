# ASSIGNMENTS COMPILATORI 23-24

# ASSIGNMENT 1:

Implementing the following local optimizations to my llvm pass:

1. Algebraic Identity: Simplifying basic equations:
    - `x + 0 = 0 + x = x -> x`
    - `x * 1 = 1 * x = x -> x`

2. Strength Reduction: Transforming an expensive operation into a simpler one:
    - `15 * x = x * 15 -> (x << 4) - x`
    - `y = x / 8 -> y = x >> 3`

3. Multi-Instruction Optimization:
    - `a = b + 1, c = a - 1 -> a = b + 1, c = b`

In the folder for the second assignment there is a pdf with the solution of 3 data flow analysis problems. 
    - `Very busy expression`
    - `Dominator analysis`
    - `Constant propagation`

# ASSIGNMENT 2:

Dataflow analysis of:

- Very busy expression

- Dominator analisys

- Constat propagation

# ASSIGNMENT 3:

Implementing optimization pass LICM - Loop invariant code motion

# ASSIGNMENT 4:

Implementing optimization pass LF - Loop fusion
