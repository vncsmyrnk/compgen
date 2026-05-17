# RFC-001: KDL CLI Completion Schema (MVP)

## 1. Objective

Define a strict, easily parsable KDL schema for generating shell completion scripts (starting with Zsh). This schema heavily aligns with the [Usage Specification](https://usage.jdx.dev/spec/) while prioritizing POSIX/GNU CLI philosophies and C-based parsing efficiency.

## 2. Core Principles

* **Context Isolation:** Subcommands create strict execution boundaries. Flags and positional arguments defined at the root do *not* inherit into subcommands.
* **Convention over Configuration:** Semantic behavior (like file vs. directory completion) should be inferred from standard naming conventions where possible, reducing the need for verbose property tags.
* **Agnosticism:** The KDL must not contain shell-specific logic (e.g., no raw Zsh scripts). It defines the *intent*; the backend generators figure out the *implementation*.

## 3. Node Definitions

### 3.1 Global Metadata

Defines the binary name and global description.

```kdl
name "mytool"
help "A sample CLI tool"

```

* **Rules:** Must appear at the root level. `name` is mandatory.

### 3.2 Flags (`flag`)

Defines optional modifiers, which can be boolean or take a value.

```kdl
flag "-v --verbose" help="Enable output" global=true multiple=true
flag "-p --port <PORT>" help="Specify port" required=true default="8080" env="PORT"

```

* **Syntax:** The string value dictates the flag format (`-short`, `--long`, `<value>`).
* **Properties (Optional):**
* `global` (bool): If `true`, the flag cascades into all nested subcommands. The generator must recursively apply this flag to child nodes.
* `multiple` (bool): Allows the flag to be provided multiple times. (In Zsh, this removes the mutual exclusion guard, changing `(-v)` to `*(-v)`).
* `required` (bool): Indicates the user must provide this flag.
* `default` (string): The fallback value. Used primarily for augmenting help text in completions.
* `env` (string): The environment variable equivalent.


* **Position:** Flags should generally precede positional arguments in the target CLI, adhering to POSIX utility conventions.

### 3.3 Positional Arguments (`arg`)

Defines required or optional positional data.

```kdl
arg "<file>" help="The target file"
arg "<action>" help="The action to perform" {
    choices "start" "stop"
}

```

* **Inference Rule:** If the argument name contains `file`, `dir`, or `path` (case-insensitive), the generator MUST provide local filesystem completions.
* **Static Choices:** Can optionally contain a `choices` block with a list of static strings.
* **Sequencing:** Multiple `arg` nodes are evaluated in the order they are defined in the KDL file.

### 3.4 Subcommands (`cmd`)

Defines a hard execution branch.

```kdl
cmd "deploy" help="Deploy the system" {
    flag "-f --force"
    arg "<target>"
}

```

* **Context Rule:** A `cmd` establishes a new parser state. The `<target>` argument above belongs *only* to `deploy`, not to the root command.
* **Recursion:** `cmd` nodes can contain their own `flag`, `arg`, and nested `cmd` nodes indefinitely.

## 4. Known Limitations & Future Scope

* **Inherited Flags:** Currently, global flags (like `--help` or `--config`) must be redefined in subcommands if the target CLI requires them post-branch. Future updates may introduce a `global=true` property.
* **Variadic Arguments:** The schema currently implies a 1:1 mapping of `arg` nodes to positions. Infinite variadic arguments (e.g., `rm file1 file2 file3`) are not explicitly tracked yet.
* **Mutually Exclusive Flags:** Not currently supported in the AST.
