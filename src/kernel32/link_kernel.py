import os, sys, inspect


def walk(path, ext):
    for root, dirs, files in os.walk(path):
        for name in files:
            if name.split(".")[-1].lower() == ext.lower():
                yield name, os.path.join(root, name), root

obj = []
path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+os.sep+"include"
for name, fullname, root in walk(path, "o"): obj.append(fullname)

path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+os.sep+"kernel"
for name, fullname, root in walk(path, "o"):
	if name == "asmkernel.o": obj.insert(0, fullname)
	else: obj.append(fullname)

path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+os.sep+"lib"
for name, fullname, root in walk(path, "a"): obj.append(fullname)

path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+os.sep
print("Linking Objects: ")
for i in obj: print(i)
gcc_lib_path = "/usr/lib/gcc/x86_64-linux-gnu/4.8/32/"
gcc_libs = ["libgcc.a", "libgcc_eh.a", "libsupc++.a", "libstdc++.a"]
for i in range(len(gcc_libs)): gcc_libs[i] = gcc_lib_path+gcc_libs[i]
os.system("ld -q -m elf_i386 -Ttext 0x30400 -o kernel32 %s %s"%(" ".join(gcc_libs), " ".join(obj)))

