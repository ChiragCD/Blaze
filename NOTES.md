Build:
make runner

Run:
./runner test.bl


Scanning:

Comments -
Single line - # This line is commented.
Multi line - """ Everything here is commented. """

Ignored characters -
' ', '\n', '\r', '\t', '\0'

Keywords -
switch, case, while, break, continue, return, true, false

Constant -
Valid numbers - 43.86, 71, .10, .35
Valid Strings - "This is a valid string.", 'So is this.', 'We ignore "double quotes" in this string.', "And 'single quotes' in this one."
Valid escape sequences in strings - "\n \t \\"

Identifier -
Allowed symbols - 0-9, a-z, A-Z, _
Can start with any of these, any sequence allowed.
Numbers will be treated as numbers (7a - identifier, 77 - number)

Additional allowed characters - (){}[]|,.


Syntax:

Block -
{
    Statement
    Statement
    ...
}

Statement -
Conditional | Loop | Block | Def | Call | continue | break | return

Conditional -
switch(Returnable){
    case(Returnable) Block
    case(Returnable) Block
    ...
}
# Case Returnable brackets optional

Loop -
while(Returnable) Block

Def -
Call Block
# Call here must have flat function name (no module.function())
# All call parameters must be unique flat identifiers

Call -
Identifier(Returnable, Returnable, ... | Returnable, Returnable, ...)

Returnable -
Call | Collection | Identifier | constant | true | false

Collection -
[Returnable, Returnable, ...]

Identifier -
identifier.identifier...


Semantics:

Entire file placed in virtual block "__main__action", under global function "__main__", present in the global namespace.
Functions are first class citizens, identified by name and number & distribution of parameters.