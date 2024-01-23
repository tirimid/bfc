# bfc

## Introduction
BFC is a brainfuck compiler capable of producing GNU-`as`-compatible assembly
files from brainfuck input files.

## Dependencies
System / software dependencies are:
* A shell environment (for program execution)
* mincbuild (for build)

## Management
* To build the program, run `mincbuild`
* To install the program after building, run `./install.sh`
* To uninstall the program after installation, run `./uninstall.sh`

## Usage
After installation, find the brainfuck file you want to compile. If this file is
called `src.bf`, you can build it as follows using BFC:

To build brainfuck source to assembly:
```
$ bfc src.bf src.s
```

To build brainfuck source to an object file (will invoke `/usr/bin/as`):
```
$ bfc -o src.bf src.o
```

To build brainfuck source to an executable binary (will invoke `/usr/bin/as` and
then `/usr/bin/ld`):
```
$ bfc -bo src.bf src
```

## Contributing
Do not contribute, I am not putting effort into supporting this project any
further unless I get a random burst of motivation to work on it. Feel free to
fork BFC and make your own version.
