[![CI Workflow](https://github.com/vncsmyrnk/compgen/actions/workflows/ci-cd.yaml/badge.svg)](https://github.com/vncsmyrnk/compgen/actions/workflows/ci-cd.yaml)
[![contributions](https://img.shields.io/badge/contributions-welcome-brightgreen?labelColor=384047&color=33cb56)](https://github.com/vncsmyrnk/shell-utils/issues)

# compgen

**Goal**: a runtime dependency-free completions generator for multiple shells configurable via document language.

**Inspiration**: [jdx/usage](https://github.com/jdx/usage). `usage` is a great tool and it is a runtime dependency.

**Secondary goals**
- Define generic parseable completion schema, based on existing [usage's KDL spec](https://usage.jdx.dev/spec/)
- Create a documentation and manpage generator

## Specification

This project's intent is to also define a specification for the completion generation. Inspiring on [usage's KDL spec](https://usage.jdx.dev/spec/), a detailed specification and scoped specifically to completion generation (ignoring help and other runtime features) is at [docs/spec.md](docs/spec.md).

## Roadmap

- [x] KDL specification for commands, flags and arguments
- [x] KDL parsing and AST implementation
- [ ] Completion generation for a particular shell, generic enough to expect future implementations for other shells
- [ ] Document usage for users
- [ ] Document requirements and guidelines and for more shells

## Building from source

```sh
make
```

## Running tests

```sh
make check
```
