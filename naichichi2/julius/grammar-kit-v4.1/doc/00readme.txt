Linux binary package of Julius-4.1.5
=====================================

This is the binary package of Julius rev. 4.1.5 for Linux. 
This archive contains pre-compiled executables, libraries, headers, 
documents and sample codes for the latest Julius.

Please see "00readme-julius.txt" for overall information of this
release, and "Release.txt" for detailed changes.

Many other documentations and resources can be found on Web.
Please access:

	http://julius.sourceforge.jp/en/

and developer's forum at:

	http://julius.sourceforge.jp/forum/


Installation
=============

Execute "./run_install.sh".  You will be asked about the target
directory.  If you leave them as default, "/usr/local" will be chosen.
When you type "y" to the confirmation, all the executables, libraries,
headers and manuals will be installed to the target directory.


----------------------------------------------------------------------
Executable binary of Julius and related tools are in "bin".  The
JuliusLib, the core engine library, is located in "lib".  The header
files are located in "include".

Directory "julius-simple" contains a sample program to compile with
JuliusLib.  It is a simplified version of Julius to start recognition
according to the given command argument as same as Julius, and output
result to stdout.  It can be compiled by the following commands.

      % cd julius-simple
      % make

The "plugin-sample" directory contains sample codes for developing
a plugin for Julius.  The content is the same as the ones included
in the original source archive.  You can compile them by "make" 
under the directory.  For further information, see the "00readme.txt"
in the directory and comments on the source codes.
