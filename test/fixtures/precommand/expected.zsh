#compdef mytool

# This script was generated automatically

function _mytool_environment_exec() {
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

function _mytool_environment() {
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
                'exec:The action to perform'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                exec)
                    _mytool_environment_exec "$@" && ret=0
                    ;;
            esac
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
                'environment:The action to perform'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                environment)
                    _mytool_environment "$@" && ret=0
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
