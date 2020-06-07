import os
import re
import sys
import sysconfig
import platform
import subprocess

from distutils.version import LooseVersion
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext


# to start developing:
# python3 setup.py build
# python3 setup.py develop


# python3 setup.py build -DBoost_PYTHON_LIBRARY_DEBUG=/usr/local/Cellar/boost-python3/1.70.0_1/lib/libboost_python37-mt.dylib -DBoost_PYTHON_LIBRARY_RELEASE=/usr/local/Cellar/boost-python3/1.70.0_1/lib/libboost_python37-mt.dylib
# python3 setup.py install -DBoost_PYTHON_LIBRARY_DEBUG=/usr/local/Cellar/boost-python3/1.70.0_1/lib/libboost_python37-mt.dylib -DBoost_PYTHON_LIBRARY_RELEASE=/usr/local/Cellar/boost-python3/1.70.0_1/lib/libboost_python37-mt.dylib
# python3 setup.py develop -DBoost_PYTHON_LIBRARY_DEBUG=/usr/local/Cellar/boost-python3/1.70.0_1/lib/libboost_python37-mt.dylib -DBoost_PYTHON_LIBRARY_RELEASE=/usr/local/Cellar/boost-python3/1.70.0_1/lib/libboost_python37-mt.dylib




# Command line flags forwarded to CMake
cmake_cmd_args = []
for f in sys.argv:
    if f.startswith('-D'):
        cmake_cmd_args.append(f)

for f in cmake_cmd_args:
    sys.argv.remove(f)



class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)',
                                         out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                cfg.upper(),
                extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        cmake_args += cmake_cmd_args

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)
        print()  # Add an empty line for cleaner output




setup(
    name='NumSrvHelper',
    version='0.1',
    author='Ignac Banič',
    author_email='ignacb@gmail.com',
    description='NumSrv module for a web server',
    long_description='',
    # tell setuptools to look for any packages under 'src'
    packages=find_packages('dist'),
    # tell setuptools that all packages will be under the 'src' directory
    # and nowhere else
    package_dir={'':'dist'},
    # add an extension module named '_NumSrvHelper' to the package 
    # 'NumSrvHelper'
    ext_modules=[CMakeExtension('NumSrvHelper/_NumSrvHelper')],
    # add custom build_ext command
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
