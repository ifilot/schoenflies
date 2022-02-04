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
import sys

# based on SemVer regex, https://semver.org/spec/v2.0.0.html#is-there-a-suggested-regular-expression-regex-to-check-a-semver-string
VERSION_REGEX = re.compile(r'## \[(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?\]')
URLS_REGEX = re.compile(r'\[(.+)\]: ')
ISSUE_REGEX = re.compile(r'#(\d)+')
MERGE_REQUEST_REGEX = re.compile(r'!(\d)+')

GITLAB_URL = os.environ.get('CI_PROJECT_URL')
ISSUE_REPL = fr'[#\1]({GITLAB_URL}/-/issues/\1)'
MERGE_REQUEST_REPL = fr'[!\1]({GITLAB_URL}/-/merge_requests/\1)'


def main():
    """Read CHANGELOG.md, extract the changelog for the provided version, and
    print to stdout.
    """
    if len(sys.argv) < 2:
        print('Provide the tag as argument!')
        sys.exit(1)

    tag = sys.argv[1]

    with open('CHANGELOG.md', 'r', encoding='utf-8') as f:
        lines = f.readlines()

    line_start = find_tag_line(lines, tag)
    if line_start is None:
        print('Tag not found in CHANGELOG.md!')
        sys.exit(1)

    line_end = find_end_line(lines, line_start)
    line_url = find_url_line(lines, tag)

    desc = ''.join(lines[line_start:line_end]).strip()
    desc = link_references(desc)

    print(desc)
    if line_url is not None:
        print('\n' + lines[line_url].strip())



def find_tag_line(lines, tag):
    for i, line in enumerate(lines):
        if re.match(VERSION_REGEX, line) and tag in line:
            return i
    return None


def find_end_line(lines, line_start):
    for i, line in enumerate(lines[line_start+1:]):  # skip first
        if re.match(VERSION_REGEX, line) or re.match(URLS_REGEX, line):
            return line_start + i + 1
    return None  # fallback, select until end


def find_url_line(lines, tag):
    for i, line in enumerate(lines):
        if match := re.match(URLS_REGEX, line):
            if tag in match[1]:
                return i
    return None


def link_references(desc):
    gitlab_url = os.environ.get('CI_PROJECT_URL')
    desc = re.sub(ISSUE_REGEX, ISSUE_REPL, desc)
    desc = re.sub(MERGE_REQUEST_REGEX, MERGE_REQUEST_REPL, desc)
    return desc


if __name__ == '__main__':
    main()
