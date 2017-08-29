# ``cwap``, the C Wrapper generator
``cwap`` is intended to generate wrappers to C/C++ code for other languages,
such as Java, C#, Python, and Ruby. The intent is to create a thin abstraction
around Clang to parse source files and determine types for export. Then to use
language-specific template engines (such as C#'s T4, Python's jija2, or Ruby's
erb) to generate C/C++ wrappers -- so we can eat our own dog food.

## Pronunciation
Its pronounced "quap", like "quack" with a "p".

# Building

## OSX
You need to use GNU-Make instead of OSX installed Make. For some reason OSX
Make does not seem to properly build intermediate dependencies.

    brew install make
    git clone 
    cd cwap
    gmake

