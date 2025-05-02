/*
Toy language:
You have a toy language grammar with primitives, tuples, generics, and functions
Primitives: char, int, float
Generices: uppercase alpha with number e.g. T1, T2 …
Tuples list of Primitives or Generic or Tuple: [ int, T1, char ] 
  - With tuple nested [int, char, [ int, T1] ]
Implement Node class to represent Primitives & Tuples (empty class provided)
  - Constructor input is value (optional str) and children (optional list of Node)
  - Value passed if primitive 
  - children passed if tuple 

Functions have params and return like this: [int; [int, T1]; T2] -> [char, T2, [float, float]]
  - Param is list of Node
  - Return is a tuple

Implement Function class to represent functions (empty class provided)
  - Constructor takes params (list of Node) and return (Node)
P1: Implement to_str for Function & Node

P2: Implement infer_return(function, param) -> return
  - Given function & param values like, return actual return type after substituting out generics (T1, T2 etc)
    - Must raise error if there is type mismatch or conflict
  - Example
    - Func: [T1, T2, int, T1] -> [T1, T2]
    - Params: [int, char, int, int]
    - Should return [int, char]
    - If params was [int, int, int, int] then raise error for type mismatch (int vs char)
    - If params was [int, int, int, char] then raise error for type conflict
*/