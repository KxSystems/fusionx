# PCRE2 Module Reference
The PCRE2 module can be loaded like so:
```
q)pcre2:use`kx.fusion:pcre2
```

## match

The PCRE2 module supports matching
```
q)text:"the cat sat on the mat"
q)pcre2.match[".(at)";text;::]
x0| "cat" 4  7  "sat" 8  11 "mat" 19 22
x1| "at" 5  7   "at" 9  11  "at" 20 22 
```

### Parameters

| Name      | Type(s)                    | Description                                                      |
|-----------|----------------------------|------------------------------------------------------------------|
| `pattern` | `string` \| `foreign`      | The pattern to match. Can also be a pre-compiled regex.          |
| `text`    | `string`                   | The text string(s) to search.                                    |
| `option`  | `symbol` \| `symbol[]`     | The options to consider: `` `caseless`` and/or `` `multiline``. The `` `caseless`` option causes case to be ignored when matching. The `` `multiline`` option causes the `^` and `$` anchors to match newlines when matching.|

### Returns

| Type   | Description                              |
|--------|------------------------------------------|
| `dict` | Pattern matched and index of pattern in text string.|

### Examples

```
q)pcre2.match["cat";"The Cat in the Hat";::]
x0|
q)pcre2.match["cat";"The Cat in the Hat";`caseless]
x0| "Cat" 4 7
```

The `multiline` option causes the `^` and `$` anchors to match newlines when matching
```
q)pcre2.match["^in";"The Cat \nin the Hat";::]
x0|
q)pcre2.match["^in";"The Cat \nin the Hat";`multiline]
x0| "in" 9 11
```
Options can be combined.
```
q)pcre2.match["^the";"The Cat in \nthe Hat";`caseless`multiline]
x0| "The" 0  3  "the" 12 15
```
## replace
The PCRE2 module supports replace.
```
q)pcre2.replace["cat";text;"dog";::]
"The dog sat on the mat"
```

### Parameters

| Name      | Type(s)                       | Description                                                      |
|-----------|-------------------------------|------------------------------------------------------------------|
| `pattern` | `string` \| `foreign`         | The pattern or compiled regex to search for.                     |
| `text`    | `string`                      | The text string(s) to search.                                    |
| `replace` | `string`                      | The pattern to replace with in the text sting.                   |
| `option`  | `symbol` \| `symbol[]`        | The options to consider: `` `caseless`` and/or `` `multiline``. The `` `caseless`` option causes case to be ignored when matching. The `` `multiline`` option causes the `^` and `$` anchors to match newlines when matching.|

### Returns

| Type       | Description                              |
|------------|------------------------------------------|
| `string`   | Text string with pattern replaced.       |


### Examples
```
q)pcre2.replace["cat";"The Cat in the Hat";"bat";::]
"The Cat in the Hat"
q)pcre2.replace["cat";"The Cat in The Hat";"bat";`caseless]
"The bat in The Hat"
```
The `multiline` option causes the `^` and `$` anchors to match newlines when replacing.
```
q)pcre2.replace["^in";"The Cat \nin the Hat";"by";::]
"The Cat \nin the Hat"
q)pcre2.replace["^in";"The Cat \nin the Hat";"by";`multiline]
"The Cat \nby the Hat"
```
Options can be combined.
```
q)pcre2.replace["^the";"The Cat in \nthe Hat";enlist"a";`caseless`multiline]
"a Cat in \na Hat"
```

## compile
You can precompile a regular expression and use it to match

### Parameters

| Name      | Type(s)               | Description                                                      |
|-----------|-----------------------|------------------------------------------------------------------|
| `pattern` | `string`              | The pattern to compile.                    |
| `option`  | `symbol` \| `symbol[]`| The options to consider: `` `caseless`` and/or `` `multiline``. The `` `caseless`` option causes case to be ignored when matching. The `` `multiline`` option causes the `^` and `$` anchors to match newlines when matching. |

### Returns

| Type      | Description                                  |
|-----------|----------------------------------------------|
| `foreign` | A pointer to a compiled PCRE2 pattern object.|


### Examples

```
q)re:pcre2.compile["cat";`caseless]
q)text:"The Cat in the Hat"
q)pcre2.match[re;text;::]
x0| "Cat" 4 
q)pcre2.replace[re;text;"Bat";::]
"The Bat in the Hat"
```
