# Expat Module Reference
Expat is a stream-oriented parser. You register callback (or handler) functions with the parser and
as the parser recognizes parts of the document it will call the appropriate handler for that part.

The Expat module can be loaded like so:
```
q)expat:use`kx.fusion:expat
```

## `create[x]`
Creates an Expat parser foreign object given a dictionary (`x`) of callback functions. The dictionary
argument should contain the following keys: all of which are optional.

|Key  |Value           |Description|
|-----|----------------|-----------|
|`seh`|Ternary function|Start element handler|
|`eeh`|Binary function |End element handler|
|`cdh`|Binary function |Character data handler|

### `seh`
The start element handler is called whenever an opening tag is encountered in the XML document. 
* `x` Parser foreign object
* `y` Tag name
* `z` attributes dictionary
```q
q)p:expat.create[([seh:{show(y;z)}])]
q)expat.parse[p;"<doc><t1 a1=\"abc\" a2=\"def\">Lorem</t1><t2>ipsum</t2></doc>";1b] 
`doc
::
`t1
(`a1;`a2)!("abc";"def")
`t2
::
1i
```

### `eeh`
The end element handler is called when a closing tag is encountered in the XML document. 
* `x` Parser foreign object
* `y` Tag name
```q
q)p:expat.create[([eeh:{show y}])]
q)expat.parse[p;"<doc><t1 a1=\"abc\" a2=\"def\">Lorem</t1><t2>ipsum</t2></doc>";1b] 
`t1
`t2
`doc
1i
```

### `cdh`
The character data handler is called when a tag contains text. 
* `x` Parser foreign object
* `y` Tag's text
```q
q)p:expat.create[([cdh:{show y}])]
q)expat.parse[p;"<doc><t1 a1=\"abc\" a2=\"def\">Lorem</t1><t2>ipsum</t2></doc>";1b]
"Lorem"
"ipsum"
1i
```

## `parse[x;y;z]`
Parses an XML string.
* `x` Expat parser foreign object
* `y` XML string
* `z` Boolean flag indicating whether this is the last part of the document
```q
q)p:expat.create[([seh:{[x;y;z]show y};eeh:{show y};cdh:{show y}])]
q)expat.parse[p;"<doc><t1 a1=\"abc\" a2=\"def\">Lorem</t1>";0b]
`doc
`t1
"Lorem"
`t1
1i
q)expat.parse[p;"<t2>ipsum</t2></doc>";1b]
`t2
"ipsum"
`t2
`doc
1i
```

## `reset[x]`
Prepare a parser object to be reused. After this has been called, the parser is ready to start
parsing a new document.
* `x` Parser foreign object

## `stop[x;y]`
Stops parsing causing `parse` to return. Must be called within a callback handler, except when
aborting (when `y` is `0b`) an already suspended parser.
* `x` Parser foreign object
* `y` Boolean flag indicating whether parsing is resumable
```q
q)seh:{[x;y;z] show y; if[y=`bar;expat.stop[x;1b]]}
q)cdh:{show y}
q)cdh:eeh:{show y}
q)p:expat.create[([seh;cdh;eeh])]
q)expat.parse[p;"<doc><foo>aaa</foo><bar>bbb</bar></doc>";1b]
`doc
`foo
"aaa"
`foo
`bar
2i
```

## `resume[x]`
Resume suspended parser. Must not be called from within a handler callback.
* `x` Parser foreign object.
```q
q)seh:{[x;y;z] show y; if[y=`bar;expat.stop[x;1b]]}
q)cdh:{show y}
q)cdh:eeh:{show y}
q)p:expat.create[([seh;cdh;eeh])]
q)expat.parse[p;"<doc><foo>aaa</foo><bar>bbb</bar></doc>";1b]
`doc
`foo
"aaa"
`foo
`bar
2i
q)expat.resume[p]
"bbb"
`bar
`doc
1i
```

## `status[x]`
Returns the status of the parser with respect to being initialized, parsing, finished, or suspended,
and whether the final buffer is being processed.
* `x` Parser foreign object.
```q
q)p:expat.create[([cdh:{show y}])]
q)expat.status[p]
`initialized
0b
q)expat.parse[p;"<doc>ab";0b]
q)expat.status[p]
`parsing
0b
q)expat.parse[p;"cd</doc>";1b]
"cd"
1i
q)expat.status[p]
`finished
1b
q)
```

## `errorCode[x]`
If `parse` returns an error, returns what type of error has occurred.
* `x` Parser foreign object.
```q
q)p:expat.create[([seh:{z;show y};cdh:{show y};eeh:{show y}])]
q)expat.parse[p;"<doc>abc<</doc>";1b]
`doc
"abc"
0i
q)expat.errorCode p
4i
```

## `errorString[x]`
Returns a string describing the error corresponding to the given error code.
* `x` Error code
```q
q)p:expat.create[([seh:{z;show y};cdh:{show y};eeh:{show y}])]
q)expat.parse[p;"<doc>abc<</doc>";1b]
`doc
"abc"
0i
q)expat.errorString expat.errorCode p
"not well-formed (invalid token)"
```

## `lineNumber[x]`
Returns the line number of the current position. The first line is reported as 1.
* `x` Parser foreign object.
```q
q)p:expat.create[([seh:{z;show y};cdh:{show y};eeh:{show y}])]
q)expat.parse[p;"<doc>\n<foo>Lorem\nipsum";0b]
`doc
,"\n"
`foo
"Lorem"
,"\n"
"ipsum"
1i
q)expat.lineNumber p
3
```

## `columnNumber[x]`
Returns the offset, from the beginning of the current line, of the current position. The first
column is reported as 0.
* `x` Parser foreign object.
```
q)p:expat.create[([seh:{z;show y};cdh:{show y};eeh:{show y}])]
q)expat.parse[p;"<doc>\n<foo>Lorem\nipsum";0b]
`doc
,"\n"
`foo
"Lorem"
,"\n"
"ipsum"
1i
q)expat.columnNumber p
5
```

## `version[]`
Returns Expat version as an integer list `(major;minor;micro)`.
```q
q)expat.version[]
2 2 9i
```

## Example
This example demonstrates how to extract data from an RSS feed. We'll use the Bank of England's News
feed.

Launch KDB-X and load the Expat module.
```q
q)expat:use`kx.fusion:expat
```
Add two global variables `news` and `s`.
```q
q)s:news:()
```
The `news` variable will hold the data extracted from the RSS feed. We'll use the `s` variable as a
stack to keep track of our position in the XML document when we are parsing it. 

Add a function `ii` to check if we're currently parsing an `item` tag.
```q
q)ii:{$[2>count s;0b;not s[1]=`item;0b;1b]}
```
We'll set up our handler callbacks so that we only extract data from the `item` tags.
```q
q)seh:{[x;y;z]s::y,s;if[y=`item;d::([])]}
q)cdh:{[x;y] if[not ii[];:`skip];d[s[0]],:y}
q)eeh:{[x;y]s::1_s;if[y=`item;news::news,enlist d]}
```
Create the parser.
```q
q)p:expat.create[([seh;cdh;eeh])]
```
Download the RSS feed.
```q
q)xml:.Q.hg `:https://www.bankofengland.co.uk/rss/news
```
If you encounter any [SSL/TLS](https://code.kx.com/q/kb/ssl) errors, you may need to set the
`KX_SSL_CA_CERT_FILE` environment variable, e.g.
```Bash
export KX_SSL_CA_CERT_FILE=/usr/lib/ssl/certs/ca-certificates.crt
```
Parse the XML document.
```q
q)expat.parse[p;xml;1b]
```
Inspect the results.
```q
q)meta news
c          | t f a
-----------| -----
guid       | C    
link       | C    
title      | C    
description| C    
pubDate    | C 
q)
q)select title from news
title                                                                                        
---------------------------------------------------------------------------------------------
"FPC review of UK bank capital \342\200\223 summary of stakeholder evidence gathering event" 
"Minutes of the SONIA Stakeholder Advisory Group - 24 March 2026"                            
"Banknote imagery advisory group minutes - October 2025"
...
```

## Links
* [Bank of England RSS Feeds](https://www.bankofengland.co.uk/rss)
* [Expat XML parser](https://libexpat.github.io/)

