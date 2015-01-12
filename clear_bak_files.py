import os, sys, inspect
path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

def walk(path, ext):
    for root, dirs, files in os.walk(path):
        for name in files:
            if name[-1] == ext:
                yield name, os.path.join(root, name), root

print("Deleting ALL files terminates with a '~' ... ")
for name, fullname, root in walk(path, "~"):
	os.system("del /F \"%s\""%fullname)
	print("DELETED: %s"%fullname)
print("Done ... ")
input()
