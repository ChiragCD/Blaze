# Blaze
Programming language and interpreter. Sample code in test.bl

Weird stuff done for fun -

Function call format is foo(arg1, arg2, ...| arg42, arg43, ...)  
&nbsp;&nbsp;&nbsp;&nbsp;where arg1, arg2 etc are passed by reference, and arg42, arg43, etc by value.  
No datatypes  
&nbsp;&nbsp;&nbsp;&nbsp;at least so far, and I intend to try to keep it that way. Chaos can be fun.

Done -

Making a scanner  
Making a parser  
Implementing namespaces  
Implementing functions  
Implementing assign and print library functions

To be done-

Implementing conditionals and loops  
Most library functions  
Cleaning up this README  

Planned Concepts -
Allow for function arguments to be filled independently, and for a call to be triggered by anyone with the function's reference.
Separate function's block and outer namespace as value, independently modifiable after a deep copy.
Make multi-interpreter multi-processing, with a separate shared namespace with a lock system handled by a server over ports.