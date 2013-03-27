# ABA #
## Analysis by ABA ##
 * Parsing into AST
 * Navigation of AST
 * Analysis of AST
 * Modification of AST
 * Serialization of AST
## Two states ##
 * Persistent storage state (state of disk, version control, ...)
 * Abstract Syntax Tree and operations on this
    * Can be in-sync with pss
    * Can be ahead of pss based on a couple of transformations (phase 2)
    * Can be easily browsed
    * Provides information for syntax highlighting
    * Can be copied into pss, making sure it can be read back

