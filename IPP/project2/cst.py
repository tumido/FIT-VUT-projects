#!/usr/bin/env python3

# CST:xcoufa09

import traceback
import os
import re
import sys
from argparse import ArgumentParser
from contextlib import contextmanager
from collections import OrderedDict
from io import StringIO

KEYWORDS = ['auto', 'break', 'case', 'char', 'const', 'continue', 'default',
            'do', 'double', 'else', 'enum', 'extern', 'float', 'for', 'goto',
            'if', 'inline', 'int', 'long', 'register', 'restrict', 'return',
            'short', 'signed', 'sizeof', 'static', 'struct', 'switch',
            'typedef', 'union', 'unsigned', 'void', 'volatile', 'while']
OPERATORS = ['++', '--', '+', '-', '+=', '-=', '*', '*=', '/', '/=', '%',
             '%=', '<', '<=', '>', '>=', '==', '!=', '!', '&&', '||', '<<',
             '<<=', '>>', '>>=', '~', '&', '&=', '|', '|=', '^', '^=', '=',
             '->', '.']

ARGUMENT_ERROR = 1
INPUT_FILE_ERROR = 2
INPUT_SUBDIR_ERROR = 21
OUTPUT_FILE_ERROR = 3


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
            self.error('unexpected option string: {0}'.format(option_string))

        # return the collected option tuples
        return result

    def error(self, message):
        self.exit(ARGUMENT_ERROR, '%s: error: %s\n' % (self.prog, message))


@contextmanager
def open_w_exit(filename, mode, exitcode):
    """
    Enhances "with open()" with sys.exit(exitcode)
    :param filename: file to open
    :param mode: specify if read, write, bytemode
    :param exitcode: when exception is raised exit with gicen code
    """
    try:
        f = open(filename, mode, encoding='iso-8859-2')
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
                        epilog='Tomas Coufal <xcoufa09@stud.fit.vutbr.cz>')
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


class SourceCode():
    """
    SourceCode object keeps the content of a file and provides methods for parsing it
    """
    def __init__(self, filename, error):
        """
        Initializes the SourceCode object with file and copies it's content
        :param filename: name of analyzed file
        :param error: error code returned if open fails
        """
        with open_w_exit(filename, 'r', error) as f:
            self.content = f.read()

    def comments_strip(self):
        """
        Removes comments and returns count of characters
        :param f: StringIO object to parse
        :return occurs: number of chars in comments
        :return content: StringIO object without comments
        """
        occurs = 0
        state = 1
        content = ''
        for char in self.content:
            # regual char
            if state == 1:
                if char == '/':
                    state = 2
                else:
                    content += char
            # suspected char
            elif state == 2:
                if char == '/':
                    occurs += 2  # doubleslash
                    state = 3
                elif char == '*':
                    occurs += 2
                    state = 4
                else:
                    state = 1
                    content += '/' + char
            # in oneline comment (till eol)
            elif state == 3:
                if char == '\n':
                    state = 1
                else:
                    occurs += 1
            # in possible multiline comment
            elif state == 4:
                if char == '*':
                    state = 5
                occurs += 1
            elif state == 5:
                state = 1 if char == '/' else 4
                occurs += 1
        self.content = content
        return occurs


    def macro_strip(self):
        content = ''
        in_macro = False
        for line in StringIO(self.content).readlines():
            if re.match('^#.*', line):
                if re.match('.*\\\$', line):
                    in_macro = True
            elif in_macro:
                if not re.match('.*\\\$', line):
                    in_macro = False
            else:
                content += line
        self.content = content

    def string_strip(self):
        self.content = re.sub('"(\\.*|[^"]*)"', '""', self.content)
        print( self.content)


    def keyword_strip(self):
        occurs = 0
        for word in KEYWORDS:
            occurs += len(re.findall('\W{0}\W'.format(word), self.content, re.IGNORECASE))
            self.content = re.sub('(\W){0}(\W)'.format(word), '\1\2', self.content)
        return occurs


    def operator_strip(self):
        content = ''
        occurs = 0
        return occurs

    def pattern_match(self, pattern):
        for line in StringIO(self.content).readlines():
            occurs += len(re.findall(pattern, line))
        return occurs

    def identificators_match(self):
        return len(re.findall('\b[a-zA-Z_]{1}\w*\b', self.content))



def find_in_file(name, mode, error):
    result = dict()
    f = SourceCode(name, error)

    if mode['w']:
        return {name: f.pattern_match(mode['w'])}
    # now strip macros
    f.macro_strip()

    occurs = f.comments_strip()
    if mode['c']:
        return {name : occurs}

    # now strip stings
    f.string_strip()

    occurs = f.keyword_strip()
    if mode['k']:
        return {name : occurs}

    occurs = f.operator_strip()
    if mode['o']:
        return {name : occurs}

    if mode['i']:
        return {name: f.identificator_match()}

    return {name : NaN}

def _list_dir(d, subdir):
    l = list()
    for item in os.listdir(d):
        path = os.path.join(d, item)
        if subdir and os.path.isdir(path):
            l += _list_dir(path, subdir)
        elif re.match('^.*\.[ch]$', item):
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
        sys.exit(INPUT_FILE_ERROR)
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
    # if no path is displayed remove it
    new = dict()
    for key, value in result_dict.items():
        if remove_path:
            key = key.split("/")[-1]
        # add indexes to handle files with the same name (up to 10 files)
        index = 0
        while key + str(index) in new.keys():
            index += 1
        key = key + str(index)
        new[key] = value

    result_dict = new

    # sort the ddictionary alphabeticaly
    result_dict = OrderedDict(sorted(result_dict.items(), key=lambda t: t[0]))

    k_max = max([len(key) for key in result_dict.keys()])
    v_max = max([len(str(val)) for key, val in result_dict.items()])
    v_sum = sum([val for key, val in result_dict.items()])

    k_max = max(k_max - 1, len("CELKEM:"))
    v_max = max(v_max, len(str(v_sum)))
    # format string
    template = "{0:" + str(k_max) + "} {1:>" + str(v_max) + "d}\n"
    buff = str()

    # add line for each file
    for key, value in result_dict.items():
        if remove_path:
            key = key.split("/")[-1]
        buff += template.format(key[:-1], value)

    buff += template.format('CELKEM:', v_sum)

    # print it out
    if output:
        with open_w_exit(output, "w", OUTPUT_FILE_ERROR) as f:
            f.write(buff)
            f.flush()
    else:
        print(buff, end='')
    return

if __name__ == "__main__":
    # Parse args
    args = parse_args()
    # List all searched files
    list_of_files = list_files(args['in'], args['sub'])
    # Compute occurencies
    result = dict()
    err = INPUT_SUBDIR_ERROR if os.path.isdir(args['in']) else INPUT_FILE_ERROR
    [result.update(find_in_file(where, args, err)) for where in list_of_files]
    # Print the result
    print_output(result, args['out'], args['p'])
