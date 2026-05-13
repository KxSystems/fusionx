# Fusionx for KDB-X

The fusionx module makes it easy to use native libraries, providing pre-built wrappers.

To use the fusionx module copy the appropriate fusionx shared object file to the `mod` directory of
your KDB-X installation. The latest release can be obtained from [here](https://github.com/KxSystems/fusionx/tags).

```Bash
unzip fusion.zip
cp fusion.li64.so $HOME/.kx/mod/kx  # the use function will search this location by default
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
q).pcre2   :use`kx.fusion:pcre2 /namespace 
q)pcre2    :use`kx.fusion:pcre2 /directory
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
The Windows binary can be cross complied on Linux using Mingw-w64.
```Bash
CC=x86_64-w64-mingw32-gcc make
```
