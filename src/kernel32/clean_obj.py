import os, sys, inspect
path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))+os.sep+"kernel"

def walk(path, ext):
    for root, dirs, files in os.walk(path):
        for name in files:
            if name.split(".")[-1].lower() == ext.lower():
                yield name, os.path.join(root, name), root
#try:
#	if int(input("0/1(1) Clean .o files? ")):
#		for name, fullname, root in walk(path, "o"):
#			os.system("rm -f %s"%fullname)
#except:
#	for name, fullname, root in walk(path, "o"):
#		os.system("rm -f %s"%fullname)
for name, fullname, root in walk(path, "o"):
	os.system("rm -f %s"%fullname)



