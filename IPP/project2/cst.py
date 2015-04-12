#!/usr/bin/env python3

# CST:xcoufa09

from argparse import ArgumentParser
from contextlib import contextmanager
from collections import OrderedDict
import traceback
import os
import re
import sys

KEYWORDS = ['auto', 'break', 'case', 'char', 'const', 'continue', 'default',
            'do', 'double', 'else', 'enum', 'extern', 'float', 'for', 'goto',
            'if', 'inline', 'int', 'long', 'register', 'restrict', 'return',
            'short', 'signed', 'sizeof', 'static', 'struct', 'switch',
            'typedef', 'union', 'unsigned', 'void', 'volatile', 'while']
OPERATORS = []


class MyArgparse(ArgumentParser):
    # An ugly hack to enable option for disabling abbreviation and force equals
    # sign when the server which this program will be tested against does not
    # offer a proper version. This code is not mine, I took it from the
    # argparse module, all credits belong to them. This solution is based on
    # advice here:
    # http://stackoverflow.com/questions/10750802/disable-abbreviation
    def _get_option_tuples(self, option_string):
        result = []

        # option strings starting with two prefix characters are only
        # split at the '='
        chars = self.prefix_chars
        if option_string[0] in chars and option_string[1] in chars:
            if '=' in option_string:
                option_prefix, explicit_arg = option_string.split('=', 1)
                option_prefix += '='  # fix the equals sign
            else:
                option_prefix = option_string
                explicit_arg = None
            for option_string in self._option_string_actions:
                if option_string == option_prefix:  # minor change gere
                    action = self._option_string_actions[option_string]
                    tup = action, option_string, explicit_arg
                    result.append(tup)

        # single character options can be concatenated with their arguments
        # but multiple character options always have to have their argument
        # separate
        elif option_string[0] in chars and option_string[1] not in chars:
            option_prefix = option_string
            explicit_arg = None
            short_option_prefix = option_string[:2]
            short_explicit_arg = option_string[2:]

            for option_string in self._option_string_actions:
                if option_string == short_option_prefix:
                    action = self._option_string_actions[option_string]
                    tup = action, option_string, short_explicit_arg
                    result.append(tup)
                elif option_string.startswith(option_prefix):
                    action = self._option_string_actions[option_string]
                    tup = action, option_string, explicit_arg
                    result.append(tup)

        # shouldn't ever get here
        else:
            self.error(_('unexpected option string: %s') % option_string)

        # return the collected option tuples
        return result


@contextmanager
def open_w_exit(filename, mode, exitcode):
    try:
        f = open(filename, mode)
    except OSError:
        traceback.print_exc(file=sys.stderr)
        sys.exit(exitcode)
    else:
        try:
            yield f
        finally:
            f.close()


def parse_args():
    """
    Function for parsing arguments, using modified version of ArgumentParser
    with disabled abbreviation of options and forces user to add equals sign
    for options with value
    :return: Dictionary full of parsed options
    """
    parser = MyArgparse(description='C stats analyzer tool',
                        epilog='Tomas Cofual <xcoufa09@stud.fit.vutbr.cz>')
    parser.add_argument('--input=', dest='in', action='store',
                        metavar='FILE or DIR', default=os.getcwd(),
                        help='specify the imput file or directory '
                        'containing files to search in')
    parser.add_argument('--output=', dest='out', action='store',
                        metavar='FILE',
                        help='output file')
    parser.add_argument('--nosubdir', dest='sub', action='store_false',
                        help='do not search in subdirectories')
    parser.add_argument('-p', action='store_true',
                        help='print filenames without absolute path')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-k', action='store_true',
                       help='count key words')
    group.add_argument('-o', action='store_true',
                       help='count operators')
    group.add_argument('-i', action='store_true',
                       help='count identificators')
    group.add_argument('-w', action='store', metavar='string',
                       help='match given string')
    group.add_argument('-c', action='store_true',
                       help='count characters of comments')

    return vars(parser.parse_args())


def find_in_file(name, mode):
    result = dict()
    occurs = 0
    with open_w_exit(name, 'r', 1) as f:
        # mode 'w' searches also in strings, macros and comments
        if mode['w']:
            for line in f.readlines():
                occurs += len(re.findall(mode['w'], line))
        # for 'c' comment count mode we need a small fsm
        elif mode['c']:
            state = 1
            for char in f.read():
                # regual char
                if state == 1:
                    state = 2 if char == '/' else 1
                # suspected char
                elif state == 2:
                    state = 3 if char == '/' else 5 if char == '*' else 1
                # in oneline comment (till eol)
                elif state == 3:
                    occurs += 2  # doubleslash
                    state = 4
                # wait for eol
                elif state == 4:
                    if char == '\n':
                        state = 1
                    else:
                        occurs += 1
                # in possible multiline comment
                elif state == 5:
                    occurs += 2  # /* at the begining
                    state = 6
                elif state == 6:
                    if char == '*':
                        state = 7
                    else:
                        occurs += 1
                elif state == 7:
                    occurs += 2
                    state = 1 if char == '/' else 6

        else:
            occurs = 5
        result[name] = occurs
    return result


def _list_dir(d, subdir):
    l = list()
    f_in_dir = os.listdir(d)
    for item in f_in_dir:
        path = os.path.join(d, item)
        if subdir and os.path.isdir(path):
            l += _list_dir(path, subdir)
        elif re.match('^.*\.[cChH]$', item):
            l.append(path)
    return l


def list_files(name, subdir):
    """
    :param name: input file or directory name
    :param subdir: set False if you don't want to search in subdirs
    :return: list of *.c *.h files
    """
    files = list()
    if os.path.isfile(name):
        files.append(name)
    elif os.path.isdir(name):
        files += _list_dir(name, subdir)
    else:
        raise IOError("File or directory does not exist")
        sys.exit(1)
    return files


def print_output(result_dict, output, remove_path):
    """
    Function for formating the output according to task specs
    :param result_dict: dictionary with filename as keys and count as value
    :param remove_path: declare whether remove path and keep just the filename
    :param output: filename or nothing of stdoutu
    """
    # compute the length of a line
    if result_dict == dict():
        return

    k_max = max([len(key) for key in result_dict.keys()])
    v_max = max([len(str(val)) for key, val in result_dict.items()])
    v_sum = sum([val for key, val in result_dict.items()])
    line = k_max + max(v_max, len(str(v_sum))) + 1
    # If you have any idea how to use the formating for this, ping me...
    template = "{0}{1}{2}\n"
    buff = str()

    # add line for each file
    result_dict = OrderedDict(sorted(result_dict.items(), key=lambda t: t[0]))
    for key, value in result_dict.items():
        if remove_path:
            key = key.split("/")[-1]
        buff += template.format(key,
                                " " * (line - len(key) - len(str(value))),
                                str(value))

    buff += template.format('CELKEM:',
                            " " * (line - len("CELKEM:") - len(str(v_sum))),
                            str(v_sum))

    # print it out
    if output:
        with open_w_exit(output, "w", 2) as f:
            f.write(buff)
            f.flush()
    else:
        print(buff)
    return

if __name__ == "__main__":
    # Parse args
    args = parse_args()
    # List all searched files
    list_of_files = list_files(args['in'], args['sub'])
    # Compute occurencies
    result = dict()
    [result.update(find_in_file(where, args)) for where in list_of_files]
    # Print the result
    print_output(result, args['out'], args['p'])
