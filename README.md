[![contributions](https://img.shields.io/badge/contributions-welcome-brightgreen?labelColor=384047&color=33cb56)](https://github.com/vncsmyrnk/shell-utils/issues)

# compgen

**Goal**: a runtime dependency-free completions generator for multiple shells configurable via document language.

**Inspiration**: [jdx/usage](https://github.com/jdx/usage). `usage` is a great tool and it is a runtime dependency.

**Secondary goals**
- Define generic parseable completion schema, based on existing [usage's KDL spec](https://usage.jdx.dev/spec/)
- Create a documentation and manpage generator

## Specification

This project's intent is to also define a specification for the completion generation. Inspiring on [usage's KDL spec](https://usage.jdx.dev/spec/), a detailed specification and scoped specifically to completion generation (ignoring help and other runtime features) is at [docs/spec.md](docs/spec.md).

## Building from source

```
make
```
