import sys
import subprocess


args = sys.argv[1:]
print(sys.argv)
subprocess.run(['glslc'] + args)
