Smoke Test
==========

## Description

This document simply describes some common use cases for using
the smoke tests found under this directory. The main idea is to
run disgnostics tools in conjunction with the smoke tests to
help determine if there are any potential issues.

## Common Use Cases
### Native
For simply just stressing the data path to see if anything blows
up just run bonefish on its own:
```
$ <bonefish>
```
### Valgrind
#### Memory Access Issues
For general memory access issues like memory underflows/overflows,
use of unitialized memory, or use of memory after being freed:
```
$ valgrind --leak-check=no <bonefish>
```

For helping further identify memory access issues like use after
free or use of uninitialized memory you can also request that the
malloc and free operations leave a signature behind:
```
$ valgrind --leak-check=no --malloc-fill=0xd0 --free-fill=0xb0 <bonefish>
```

#### Memory Leaks
To track memory leaks:
```
$ valgrind --leak-check=<summary|full> <bonefish>
```

### Buffer Overflows/Underflows
To check for buffer overflows/underflows:
```
$ valgrind --leak-check=no <bonefish>
