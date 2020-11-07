import logging
import argparse
import os
import subprocess
import json
import sys
from time import sleep
from os import path


class BinderError(Exception):
    pass


class RunCmdError(BinderError):

    MAX_OUTPUT_LEN = 5000

    def __init__(self, cmd, stdout, stderr, exit_code):
        super(RunCmdError, self).__init__(
            'Command: {cmd}\n'
            'Exit code: {exit_code}\n'
            'Stdout:\n{stdout}\n'
            'Stderr:\n{stderr}'.format(
                cmd=cmd,
                exit_code=exit_code,
                stdout=stdout[-self.MAX_OUTPUT_LEN:],
                stderr=stderr[-self.MAX_OUTPUT_LEN:]))


class Settings(object):

    def __init__(self, argv):
        self.__args = self.__parse_argv(argv=argv)

    @classmethod
    def __parse_argv(cls, argv):
        parser = argparse.ArgumentParser(
            description='Generates das::Module binding stuff from .h file.')
        parser.add_argument('--c_header_from', type=str, required=True,
            help='.h file to generate bindings from.')
        parser.add_argument('--module_to', type=str, required=True,
            help='.cpp file to write generated das::Module to.')
        parser.add_argument('--clang_c_exe', type=str, default='clang',
            help='Clang C compiler to use. Default: %(default)s')
        parser.add_argument('--include_dirs', type=str,
            help='Additional "include" directories to use.')
        parser.add_argument('--include_dirs_sep', type=str, default=';',
            help='Separator used in "--include_dirs".')
        parser.add_argument('--config', type=str, required=True,
            help='Path to binding config.')
        parser.add_argument('--log_level', type=str,
            choices=['debug', 'info', 'warning', 'error'],
            default='info', help='Logging level. Default: %(default)s')
        return parser.parse_args(argv)

    @property
    def log_level(self):
        return getattr(logging, self.__args.log_level.upper())

    @property
    def module_to(self):
        return full_path(self.__args.module_to)

    @property
    def c_header_from(self):
        return full_path(self.__args.c_header_from)

    @property
    def clang_c_exe(self):
        return self.__args.clang_c_exe

    @property
    def include_dirs(self):
        return self.__args.include_dirs.split(self.__args.include_dirs_sep)

    @property
    def config_fpath(self):
        return full_path(self.__args.config)


class LoggingObject(object):

    def _log_info(self, msg):
        self.__log(level='info', msg=msg)

    def _log_debug(self, msg):
        self.__log(level='debug', msg=msg)

    @property
    def _log_namespaces(self):
        return []

    @property
    def __logger_id(self):
        return '.'.join(map(str,
            [self.__class__.__name__] + self._log_namespaces +
            [f'obj_{id(self)}']))

    def __log(self, level, msg):
        logger = logging.getLogger(self.__logger_id)
        log_fn = getattr(logger, level)
        log_fn(msg)


class Binder(LoggingObject):

    def __init__(self, argv):
        self.__settings = Settings(argv=argv[1:])
        self.__config = self.__read_config(self.__settings.config_fpath)
        self.__c_header = C_AST(
            c_src_fpath=self.__settings.c_header_from,
            clang_c_exe=self.__settings.clang_c_exe,
            include_dirs=self.__settings.include_dirs,
            config=self.__config)

    def run(self):
        logging.basicConfig(level=self.__settings.log_level,
            format='%(asctime)s [%(levelname)s:%(name)s] %(message)s')
        self._log_info(f'Generating bindings for '
            f'{self.__settings.c_header_from}')
        write_to_file(fpath=self.__settings.module_to,
            content='\n'.join(self.__generate_module() + ['']))
        self._log_info(f'Wrote generated das::Module to '
            f'{self.__settings.module_to}')
        self._log_info('Finished successfully.')

    def __read_config(self, config_fpath):
        cfg_globals = {}
        try:
            with open(config_fpath, 'r') as f:
                cfg_py = f.read()
        except IOError:
            raise BinderError(f'Could not read config file: {config_fpath}')
        old_sys_path = list(sys.path)
        try:
            sys.path.append(full_path(path.dirname(__file__)))
            exec(cfg_py, cfg_globals)
        finally:
            sys.path = old_sys_path
        config_class = cfg_globals.get('Config')
        if config_class is None:
            raise BinderError(f'Config file must define "Config" class.')
        return config_class()

    def __app_name(self):
        return path.basename(path.dirname(__file__))

    def __generate_module(self):
        lines = []
        module = self.__config.das_module_name
        include = self.__config.c_header_include
        lines += [
           f'// generated by {self.__app_name()}',
            '',
            '#include "daScript/daScript.h"',
            '',
           f'#include "{include}"',
            '',
            'using namespace das;',
            '',
        ]
        lines += [line for enum in self.__c_header.enums
            for line in enum.generate_decl()]
        lines += [
            '',
           f'class Module_{module} : public Module {{',
            'public:',
           f'    Module_{module}() : Module("{module}") {{',
            '        ModuleLibrary lib;',
            '        lib.addModule(this);',
            '        lib.addBuiltInModule();'] + [
           f'        {line}' for enum in self.__c_header.enums
                        for line in enum.generate_add()] + [
            '    }',
            '};',
            '',
            f'REGISTER_MODULE(Module_{module});',
        ]
        return lines


class C_AST(object):

    def __init__(self, c_src_fpath, clang_c_exe, include_dirs, config):
        cmd = []
        cmd += [clang_c_exe, '-cc1', '-ast-dump=json']
        for dpath in include_dirs:
            cmd += [f'-I{dpath}']
        cmd += [c_src_fpath]
        out, err, exit_code = run_exec(cmd)
        self.__ast = json.loads(out)
        self.__config = config

    @property
    def enums(self):
        for inner in self.__ast['inner']:
            if inner['kind'] == 'EnumDecl':
                enum = C_Enum(root=inner)
                self.__config.configure_enum(enum)
                if not enum.is_ignored:
                    yield enum


class C_Enum(object):

    def __init__(self, root):
        assert_equal(root['kind'], 'EnumDecl')
        self.__root = root
        self.__ignored = False

    @property
    def name(self):
        return self.__root['name']

    @property
    def fields(self):
        for inner in self.__root['inner']:
            if inner['kind'] == 'EnumConstantDecl':
                yield inner['name']

    def ignore(self):
        self.__ignored = True

    @property
    def is_ignored(self):
        return self.__ignored

    def generate_decl(self):
        lines = []
        lines += [f'DAS_BIND_ENUM_CAST({self.name});']
        lines += [f'DAS_BASE_BIND_ENUM({self.name}, {self.name}']
        lines += [f',   {f}' for f in self.fields]
        lines += [')']
        return lines

    def generate_add(self):
        return [f'addEnumeration(make_smart<Enumeration{self.name}>());']


def make_dirs(dpath):
    MAX_TRIES = 100
    for _ in range(MAX_TRIES):
        try:
            os.makedirs(dpath)
        except FileExistsError:
            pass
        if path.isdir(dpath):
            break
        sleep(0.1)
    else:
        raise BinderError(f'Cannot create directory: {dpath}')

def write_to_file(fpath, content):
    make_dirs(path.dirname(fpath))
    with open(fpath, 'w') as f:
        f.write(content)

def run_exec(cmd, raise_on_error=True):
    result = subprocess.run(cmd, shell=False, capture_output=True)
    stdout = result.stdout.decode()
    stderr = result.stderr.decode()
    exit_code = result.returncode

    if raise_on_error and exit_code != 0:
        raise RunCmdError(cmd=cmd, stdout=stdout, stderr=stderr,
            exit_code=exit_code)

    return stdout, stderr, exit_code

def full_path(p):
    return path.realpath(path.abspath(p))

def assert_equal(a, b):
    if a != b:
        raise AssertionError('{} != {}'.format(repr(a), repr(b)))

def assert_not_equal(a, b):
    if a == b:
        raise AssertionError('{} == {}'.format(repr(a), repr(b)))


if __name__ == '__main__':
    Binder(argv=sys.argv).run()
