# GUBG: Generic Utilities by GUBG #

## Introduction ##

This software package implements general functionality:

* Artificial intelligence: not so much is actually there yet, but it will come
* Build, test and document: fff is a generic build tool that can compile and run tests, and in the future can create documentation
* Visualization
* Gaming and physics
* Configuration files for gvim, mucommander, emacs, bash etc.

## Getting started ##

### Linux ###

* Install rake
* Clone/extract the gubg repo somewhere

```bash
git clone https://github.com/gfannes/gubg
```

* Export GUBG to this dir and source bash/bash.sh from your .bashrc, e.g. by appending to following at the bottom of .bashrc:

```bash
export $GUBG=$HOME/gubg
. $GUBG/bash/bash.sh
```

* Build fff

```bash
cd $GUBG
rake fff
```

### Windows ###

* Install [MSYS2](https://msys2.github.io/)
* Install rake
```bash
pacman -S rake
```
* Follow the rest of the linux procedure

## Languages ##

* [C++](http://cppreference.com): Current development is mainly for C++. Some of the earlier code was also written in C++, after some experimentation with D and D2, all effort is back on C++.
* [Rust](http://rust-lang.org) seems very interesting
* [Ruby](http://ruby-lang.org): Great for doing various things
* [D](http://www.digitalmars.com/d) seemed great at the time, but it is not part of the plan anymore

## Platforms ##

* Linux/GNU: Arch linux, Ubuntu, LinuxMint and Kubuntu were tested
* Win32 was less supported originally but it is catching up

## Compilers ##

* g++ 4.8.2
* Microsoft cl 18 (Visual Studio 2013)

## Support ##

None at all, any questions, comments or suggestions are always welcome at geert_dot_fannes_at_gmail_dot_com