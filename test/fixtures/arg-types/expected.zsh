#compdef mytool

# This script was generated automatically

function _mytool_stop() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1:Container ID:_guard "[0-9]#"' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_cp() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1:container:->action_container' \
        '2:container:->action_container' \
        '*:: :->args' && ret=0

    case $state in
        action_container)
            local -a choices
            choices=(${(f)"$(_call_program container docker container ls -a 2>/dev/null)"})
            compadd -a choices && ret=0
            ;;
        action_container)
            local -a choices
            choices=(${(f)"$(_call_program container docker container ls -a 2>/dev/null)"})
            compadd -a choices && ret=0
            ;;
    esac

    return ret
}

function _mytool_checkout() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1:branch-or-tree-ish-or-file:->action_branch-or-tree-ish-or-file' \
        '*:: :->args' && ret=0

    case $state in
        action_branch-or-tree-ish-or-file)
            local -a choices
            choices=(${(f)"$(_call_program branch-or-tree-ish-or-file git for-each-ref --format='"%(refname)"' refs/heads 2>/dev/null)"})
            compadd -a choices && ret=0
            ;;
    esac

    return ret
}

function _mytool_run() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1:image:->action_image' \
        '2:string:' \
        '*:: :->args' && ret=0

    case $state in
        action_image)
            local -a choices
            choices=(${(f)"$(_call_program image docker container ls -a 2>/dev/null)"})
            compadd -a choices && ret=0
            ;;
    esac

    return ret
}

function _mytool_generate() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1:shell:->action_shell' \
        '*:: :->args' && ret=0

    case $state in
        action_shell)
            local -a choices
            choices=('zsh' 'bash')
            compadd -a choices && ret=0
            ;;
    esac

    return ret
}

function _mytool_exec() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '*:: :->args' && ret=0

    case $state in
        args)
            _normal
            ;;
    esac

    return ret
}

function _mytool_mv() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1:Source file:_files' \
        '2:Destination file:_files' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_add() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '*:Files to be staged:_files' && ret=0

    return ret
}

function _mytool() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '1: :->action' \
        '*:: :->args' && ret=0

    case $state in
        action)
            local -a choices
            choices=(
                'stop:Stop one or more running containers'
                'cp:Copy files/folders between a container and the local filesystem'
                'checkout:Switch branches or restore working tree files'
                'run:Create and run a new container from an image'
                'generate:Generate a completion file'
                'exec:The action to perform'
                'mv:Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY'
                'add:Add contents  of  new or changed files to the index'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                stop)
                    _mytool_stop "$@" && ret=0
                    ;;
                cp)
                    _mytool_cp "$@" && ret=0
                    ;;
                checkout)
                    _mytool_checkout "$@" && ret=0
                    ;;
                run)
                    _mytool_run "$@" && ret=0
                    ;;
                generate)
                    _mytool_generate "$@" && ret=0
                    ;;
                exec)
                    _mytool_exec "$@" && ret=0
                    ;;
                mv)
                    _mytool_mv "$@" && ret=0
                    ;;
                add)
                    _mytool_add "$@" && ret=0
                    ;;
            esac
            ;;
    esac

    return ret
}

if [ "$funcstack[1]" = "_mytool" ]; then
    _mytool "$@"
else
    compdef _mytool mytool
fi
