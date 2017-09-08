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

# Relationship to SWIG
I'm a huge fan of SWIG, and have used it successfully in other projects.

One thing I'd like to do differently would be to define/detect specific interfaces (iterable, indexable, List,
Dictionary, etc.). Given a set of interfaces, the various implementations should be able to share some common wrapper
code to facilitate the language-specific implementations.

I have had some problems with using smart pointers and using multiple references with SWIG. One example is that when
getting items from a dictionary will return unique native item wrappers even though the underlying object is the same --
though, to be honest, I'm not sure how to fix that. Likewise, I've also had issues when creating object instances that
have directors within C/C++. This was an issue for me in MPI Action Framework, where a Python subclass was being
instantiated from C++ and would later be deleted from Python even though the underlying class was still being used.

Lastly, I'm not a fan of the interface file requirement. For me, I generally want my entire projec to be usable, so
requring the interface file is a bit of a pain. Likewise, requiring smart pointers for all subclasses, etc. is less than
ideal for my needs.

