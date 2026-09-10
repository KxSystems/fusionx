# Fusionx for KDB-X

The fusionx module makes it easy to use native libraries, providing pre-built wrappers.

## Install with qmamba

qmamba is a package manager for KDB-X. It is currently available in a private preview capacity. You are welcome to try it and provide feedback.
Follow the install instructions for [qmamba](https://github.com/KxSystems/qmamba/blob/main/README.md#installation).

It's advisable to install the packages for your project in a separate environment, e.g.
```q
qmamba:use`kx.qmamba
qmamba.create "myenv"
qmamba.activate "myenv"
```

## Install a release

To use the fusionx module copy the appropriate fusionx shared object file to the `mod` directory of
your KDB-X installation. The latest release can be obtained from [here](https://github.com/KxSystems/fusionx/tags).

```Bash
unzip fusion-l64.zip -d ~/.kx/mod
```
List available modules.
```q
q)fusion:use`kx.fusion
q)fusion.libs[]
``blas`pcre2`expat
```
Every module is a dictionary of functions.
```q
q)use`kx.fusion:pcre2
version| code
match  | code
replace| code
compile| code
```
Fusionx purposefully doesn't interact with q namespaces, leaving the choice to you.
```q
q).pcre2   :use`kx.fusion:pcre2 /directory 
q)pcre2    :use`kx.fusion:pcre2 /namespace
q)([match]):use`kx.fusion:pcre2 /one function
```
Fusionx is built once and supports all distros, library versions, etc. you never have
to (re)compile it. see `docker.q` for how we test it with different distributions.

Adding modules to fusionx is easy. Add a directory for it and put in `modules :=` in
the Makefile. See any of the existing ones (e.g. `pcre2/pcre2.c`) for an example.

To build on Linux or macOS simply invoke Make.
```Bash
make
```
The Windows binary can be cross compiled on Linux using Mingw-w64.
```Bash
CC=x86_64-w64-mingw32-gcc make
```

## Conda package

For information on how to build and host a conda package see [`Conda usage`](https://github.com/KxSystems/qmamba/wiki/Build)

## Notice

Copyright (c) 2026 KX Systems, Inc.

Licensed under the Apache License, Version 2.0.
