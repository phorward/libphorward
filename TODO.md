# Todo

This is the location for ideas and modifications with an unspecified roadmap.

## base

- Info / logging system for status, warning, error messages related to the usage
  of functions (beyond debug!)

## regex

- Remove ``pregex_accept`` and replace occurences by just ``accept`` and
  ``flags``, also to enable flags on given contexts only (greedyness)
- Revise greedyness/non-greedyness handling according to exising implementations
- Regex-parser implemented using pparse

## parse

- Precedence and associativity leveling to resolve conflicts
- Direct traversal from an AST into a
- GLR parser with on-line conflict resolution (target shall still be one AST!)
