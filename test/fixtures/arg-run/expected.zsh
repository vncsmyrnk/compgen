#compdef mytool

# This script was generated automatically

function _mytool_checkout() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '(--detach)'--detach'[detach the HEAD at named commit]' \
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
                'checkout:Switch branches or restore working tree files'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                checkout)
                    _mytool_checkout "$@" && ret=0
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
