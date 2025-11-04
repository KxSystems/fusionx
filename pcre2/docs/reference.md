# PCRE2 Module Reference
The PCRE2 module can be loaded like so:
```
q)pcre2:use`fusion:pcre2
```
The PCRE2 module supports matching
```
q)text:"the cat sat on the mat"
q)pcre2.match[".(at)";text;::]
x0| "cat" 4  7  "sat" 8  11 "mat" 19 22
x1| "at" 5  7   "at" 9  11  "at" 20 22 
```
and replacing.
```
q)pcre2.replace["cat";text;"dog";::]
"The dog sat on the mat"
```
The last argument to the `match` and `replace` functions is for additional options. The `caseless`
option causes case to be ignored when matching
```
q)pcre2.match["cat";"The Cat in the Hat";::]
x0|
q)pcre2.match["cat";"The Cat in the Hat";`caseless]
x0| "Cat" 4 7
```
or replacing.
```
q)pcre2.replace["cat";"The Cat in the Hat";"bat";::]
"The Cat in the Hat"
q)pcre2.replace["cat";"The Cat in The Hat";"bat";`caseless]
"The bat in The Hat"
```
The `multiline` option causes the `^` and `$` anchors to match newlines when matching
```
q)pcre2.match["^in";"The Cat \nin the Hat";::]
x0|
q)pcre2.match["^in";"The Cat \nin the Hat";`multiline]
x0| "in" 9 11
```
or replacing.
```
q)pcre2.replace["^in";"The Cat \nin the Hat";"by";::]
"The Cat \nin the Hat"
q)pcre2.replace["^in";"The Cat \nin the Hat";"by";`multiline]
"The Cat \nby the Hat"
```
Options can be combined.
```
q)pcre2.match["^the";"The Cat in \nthe Hat";`caseless`multiline]
x0| "The" 0  3  "the" 12 15
q)pcre2.replace["^the";"The Cat in \nthe Hat";enlist"a";`caseless`multiline]
"a Cat in \na Hat"
```