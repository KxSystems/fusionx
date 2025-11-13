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
``blas`pcre2`
```
Every module is a dictionary of functions.
```q
q)use`kx.fusion:pcre2
version| `.m.fusion.pcre2.export.version[]
match  | `.m.fusion.pcre2.export.match[]
replace| `.m.fusion.pcre2.export.replace[]
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
