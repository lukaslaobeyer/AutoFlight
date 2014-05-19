import fileinput
import subprocess
import sys

command = ['git', 'rev-parse', '--short', 'HEAD']
p = subprocess.Popen(command, stdout=subprocess.PIPE)
text = p.stdout.read()
retcode = p.wait()

build_number = text.decode(encoding='UTF-8')[:-1];


filename = '../src/afconstants.h';

if build_number in open(filename).read():
    print('[INFO] Build number already updated')
    sys.exit()

print("[INFO] Updating build number")

for line in fileinput.input([filename], inplace=True):
    if 'const std::string BUILD_NUMBER' in line:
        new_line = '\tconst std::string BUILD_NUMBER     = "'+ text.decode(encoding='UTF-8')[:-1] +'";\n'
        sys.stdout.write(new_line)
    else:
        sys.stdout.write(line)
