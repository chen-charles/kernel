makefile_header = \
"""
CC	=	gcc
CFLAGS	=	-w -m32 -masm=intel -std=c11 -fno-stack-protector -c
"""
import os, sys, inspect

#sep = input("os.sep for including path: ")
sep = 0
if sep == "\\" or sep == "/": pass
else: sep = os.sep
print("Using Sep: %s"%sep)
def parse_include(path, name, sep=os.sep, include_stat="#include"):
	with open(path+sep+name, "r") as f:
		for i in f.readlines():
			if include_stat in i:
				if '<' in i or '>' in i:
					continue
				elif '\'' in i:
					t = '\''
				elif '\"' in i:
					t = '\"'
				else:continue
				i = i[i.index(t)+1:]
				i = i[:i.index(t)]

				li = i.split(sep)
				p = path.split(sep)
				for i in li:
					if i == "..":
						p.pop()
					elif i == ".":
						pass
					else:
						p.append(i)
				yield sep.join(p)
				for j in parse_include(sep.join(p[:-1]), p[-1], sep, include_stat): yield j

				



path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

def walk(path, ext):
    for root, dirs, files in os.walk(path):
        for name in files:
            if name.split(".")[-1].lower() == ext.lower():
                yield name, os.path.join(root, name), root

all_name = []
c_data = []
for name, fullname, root in walk(path, "c"):
	inc = ["%s "%fullname]
	all_name.append(fullname[:-2]+".o")
	for i in parse_include(root, name, sep):
		inc.append("%s "%i)
	s = "%s.o: %s\n\t$(CC) $(CFLAGS) $< -o $@\n"%(fullname[:-2], "".join(inc))
	c_data.append(s)

f = open("Makefile", "w")
f.write(makefile_header+"\nall: %s\n\n"%" ".join(all_name)+"".join(c_data))
f.close()
