# Schoenflies
# Copyright (c) 2022 Luuk Kempen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

import os
import re
import shutil
import sys
from pathlib import PurePosixPath

PATH_REGEX = re.compile(r'"(.+)"')


def main():
    """Read a Wine log file and copy all non-Windows dll files required by the
    program.
    """
    if len(sys.argv) < 2:
        print('Provide the path to a file containing Wine\'s log as argument!')
        sys.exit(1)

    filepath = sys.argv[1]

    if not os.path.isfile(filepath):
        print('Provided path is not a file!')
        sys.exit(1)

    with open(filepath, 'r') as f:
        log = f.readlines()

    paths = parse_log(log)
    copy_paths(paths)


def parse_log(log):
    paths = []
    for line in log:
        parsed_line = parse_line(line)
        if parsed_line is not None:
            paths.append(parsed_line)
    return paths


def parse_line(line):
    if not ('build_module' in line and 'native' in line):
        return None

    result = PATH_REGEX.search(line)
    path = result.group(1)

    # convert Wine Windows path to POSIX path
    path = path.replace(r'\\', '/').replace('Z:', '')
    src = PurePosixPath(path)

    if 'plugins' in src.parts:
        # Qt plugin dlls should be in a subdirectory
        index = src.parts.index('plugins')
        plugins_path = PurePosixPath(*src.parts[:index + 1])
        dst = src.relative_to(plugins_path)
    else:
        dst = PurePosixPath(src.name)

    return src, dst


def copy_paths(paths):
    for src, dst in paths:
        print(f'{src}\n  --> {dst}')
        os.makedirs(dst.parent, exist_ok=True)
        shutil.copy2(src, dst)


if __name__ == '__main__':
    main()
