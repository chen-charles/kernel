makefile_header = \
"""
ASM	=	nasm
ASMFLAGS	=	-f elf -i
"""
import os, sys, inspect

#sep = input("os.sep for including path: ")
sep = 0
if sep == "\\" or sep == "/": pass
else: sep = os.sep
print("Using Sep: %s"%sep)
def parse_include_asm(path, name, sep=os.sep, include_stat="#include"):
	if name[-3:] != "asm": 
		path = incpath
	with open(path+sep+name, "r") as f:
		try:	
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
				
					yield incpath+sep+p[-1].strip()
					for j in parse_include_asm(path, p[-1], sep, include_stat): yield j
		except:
			input("Exception occured while parsing: %s"%path+sep+name)
			raise

				



path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
incpath = path+sep+"include"
def walk(path, ext):
    for root, dirs, files in os.walk(path):
        for name in files:
            if name.split(".")[-1].lower() == ext.lower():
                yield name, os.path.join(root, name), root

all_name = []

asm_data = []
for name, fullname, root in walk(path, "asm"):
	inc = ["%s "%fullname]
	all_name.append(fullname[:-4]+".o")
	for i in parse_include_asm(root, name, sep, "%include"):
		inc.append("%s "%i)
	s = "%s.o: %s\n\t$(ASM) $(ASMFLAGS) %s -o $@ $<\n"%(fullname[:-4], "".join(inc), incpath+sep)
	asm_data.append(s)

f = open("Makefile", "w")
f.write(makefile_header+"\nall: %s\n\n"%" ".join(all_name)+"".join(asm_data))
f.close()

