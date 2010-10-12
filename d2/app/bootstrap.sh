#!/bin/sh
dmd -ofgb-bootstrap gb.d Collection.d Configuration.d MetaInfo.d Commands.d Exit.d ../gubg/Tree.d ../gubg/FSTree.d ../gubg/JSON.d ../gubg/Build.d ../gubg/Format.d ../gubg/FileCache.d ../gubg/OnlyOnce.d ../gubg/OptionParser.d ../gubg/parsing/D.d -L--export-dynamic
