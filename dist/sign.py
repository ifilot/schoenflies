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

import argparse
import glob
import os
import shutil
import subprocess
import sys
from pathlib import PurePosixPath


def main():
    """Sign dll and exe files using either osslsigncode (Linux) or signtool
    (Windows).
    """
    ap = argparse.ArgumentParser(prog='sign',
                                 description='Sign dll and exe files.',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    ra = ap.add_argument_group('required arguments')
    ra.add_argument('--cert', required=True,
                    help='PKCS#12 container with certificate and key')
    ap.add_argument('--pass', dest='password',  # `pass` is a Python keyword
                    help='PKCS#12 key password')
    ap.add_argument('--ts', dest='timestamp', default='http://timestamp.digicert.com',
                    help='URL of the timestamp server')
    ap.add_argument('files', nargs='*',
                    help='Files to sign. Leave empty to recursively sign all files in working directory.')

    args = ap.parse_args()

    check_cert(args.cert)
    files = get_files(args.files)

    sign_files(args, files)


def check_cert(path):
    if not os.path.isfile(path):
        print('The provided path to the certificate container is not a file!')
        sys.exit(1)


def get_files(files):
    if len(files) == 0:
        # recursively find all dll and exe files
        files = glob.glob('**/*.exe', recursive=True) + glob.glob('**/*.dll', recursive=True)
    else:
        # check for each file if it exists
        error = False
        for path in files:
            if not os.path.isfile(path):
                print(f'The provided path {path} is not a file!')
                error = True
        if error:
            sys.exit(1)

    return files


def sign_files(args, files):
    if sys.platform.startswith('win32'):
        sign_function = sign_file_windows
    elif sys.platform.startswith('linux'):
        sign_function = sign_file_linux
    else:
        print('This script can only be ran on Windows and Linux.')
        sys.exit(1)

    for i, path in enumerate(files):
        success = sign_function(args, path)
        if success:
            print(f'Signed {path}')
        else:
            print(f'Error when signing {path}')
            sys.exit(1)


def sign_file_windows(args, path):
    command = [
        'signtool', 'sign',
        '/f', args.cert,
        '/fd', 'sha256',
        '/tr', args.timestamp,
        '/td', 'sha256'
    ]
    if args.password:
        command += ['/p', args.password]
    command.append(path)

    proc = run_process(command)
    return proc.returncode == 0


def sign_file_linux(args, path):
    path_obj = PurePosixPath(path)
    temp_path = path_obj.parent / f'{path_obj.stem}-signed{path_obj.suffix}'
    command = [
        'osslsigncode', 'sign',
        '-pkcs12', args.cert,
        '-t', args.timestamp,
        '-in', path,
        '-out', temp_path
    ]
    if args.password:
        command += ['-pass', args.password]

    proc = run_process(command)
    shutil.move(temp_path, path)
    return proc.returncode == 0


def run_process(command):
    proc = subprocess.run(command, capture_output=True, encoding='utf-8')
    if proc.returncode != 0:
        print('Error:')
        print(proc.stdout)
        print(proc.stderr)
    return proc


if __name__ == '__main__':
    main()
