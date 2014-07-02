

import os, sys, re

include_matcher = re.compile('[#|%]include\s*[<|"](.*)[>|"]')
include_dirs = sys.argv[2].split(';')

def get_swig_incs(file_path):
    file_contents = open(file_path, 'r').read()
    return include_matcher.findall(file_contents, re.MULTILINE)

def get_swig_deps(file_path, level):
    deps = [file_path]
    if level == 0: return deps
    for inc_file in get_swig_incs(file_path):
        for inc_dir in include_dirs:
            inc_path = os.path.join(inc_dir, inc_file)
            if not os.path.exists(inc_path): continue
            deps.extend(get_swig_deps(inc_path, level-1))
    return deps

if __name__ == '__main__':
    ifiles = sys.argv[1].split(';')
    deps = sum([get_swig_deps(ifile, 3) for ifile in ifiles], [])
    #sys.stderr.write(';'.join(set(deps)) + '\n\n')
    print(';'.join(set(deps)))
