expat:use`$"fusion:",string M
p:expat.create[([cdh:{r::y}])]
expat.parse[p;"<foo>Hello</foo>";1b];
t)"Hello"~r
r:();p:expat.create[([seh:{[x;y;z]r,:y};eeh:{r,:y}])]
expat.parse[p;"<doc><foo></foo><bar></bar></doc>";1b];
t)`doc`foo`foo`bar`bar`doc~r
p:expat.create[([seh:{[x;y;z]r::z}])]
expat.parse[p;"<foo a=\"abc\" b=\"def\"></foo>";1b];
t)(`a`b!("abc";"def"))~r
t)1b~expat.reset[p]
t)0i~expat.parse[p;"<doc>abc<</doc>";1b]
t)4i~expat.errorCode p
t)"not well-formed (invalid token)" ~ expat.errorString expat.errorCode p
t)(6h;3)~(type;count) @\: expat.version[]
r:();p:expat.create[([seh:{[x;y;z]$[y=`bar;expat.stop[x;1b];r,:y]};eeh:{r,:y}])]
t)2i~expat.parse[p;"<doc><foo></foo><bar></bar></doc>";1b]
t)`doc`foo`foo~r
t)(`suspended;1b)~expat.status p
t)1i~expat.resume[p]
t)`doc`foo`foo`bar`doc~r
p:expat.create[([])];
expat.parse[p;"<doc>\n<foo>Lorem\nipsum";0b];
t)3~expat.lineNumber p
t)5~expat.columnNumber p
