import os
import re
import sys
import platform
import subprocess
import glob
import setuptools

from setuptools import setup, Extension
from setuptools import Extension
from setuptools.command.build_ext import build_ext, copy_file

from distutils.version import LooseVersion

MIN_SETUPTOOLS_VERSION = "31.0.0"
assert (LooseVersion(setuptools.__version__) >= LooseVersion(MIN_SETUPTOOLS_VERSION)), "pyAMI requires a setuptools version '{}' or higher (pip install setuptools --upgrade)".format(MIN_SETUPTOOLS_VERSION)

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))

import pathlib

class Module(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(os.path.join(CURRENT_DIR))


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        #if platform.system() == "Windows":
        #    cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
        #    if cmake_version < '3.1.0':
        #        raise RuntimeError("CMake >= 3.1.0 is required on Windows")
        for ext in self.extensions:
            self.build_extension(ext)
        self.copy_extensions_to_source()


    def build_extension(self, ext):

        fullname = self.get_ext_fullname(ext.name)
        filename = self.get_ext_filename(fullname)

        source_dir                     = ext.sourcedir
        build_temp                     = self.build_temp
        extdir                         = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_library_output_directory = os.path.abspath(os.path.dirname(build_temp))
        cmake_args = [
            '-DAMI_USE_CCACHE=on',
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={}'.format(cmake_library_output_directory),
            '-DPYTHON_EXECUTABLE={}'.format(sys.executable),
        ]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), cmake_library_output_directory)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]

        env = os.environ.copy()

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

        pyAMI_dst  = os.path.join(self.build_lib, self.get_ext_filename(self.get_ext_fullname(ext.name)))

        libsuffix = pyAMI_dst.split(".")[-1]
        pyAMI_path = os.path.join(cmake_library_output_directory, "pyAMI.{}".format(libsuffix))
        if not os.path.exists(self.build_lib):
            os.makedirs(self.build_lib)

        copy_file(
                pyAMI_path, pyAMI_dst, verbose=self.verbose,
                dry_run=self.dry_run)


setup(
    ext_modules=[Module('pyAMI')],
    cmdclass=dict(build_ext=CMakeBuild),
)
