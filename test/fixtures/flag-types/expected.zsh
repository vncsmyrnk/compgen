#compdef mytool

# This script was generated automatically

function _mytool_checkout() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '*'-i'=[Variadic short-only file flag example]:file:_files' \
        '*'--multiple-files'=[Variadic long-only file flag example]:file:_files' \
        '*'{-m,--file-mult}'=[Variadic file flag example]:file:_files' \
        --name'=[Name]:string:' \
        -V'[Verbose output]' \
        '(-s --shell)'{-s,--shell}'=[Target shell]:shell:(zsh bash)' \
        '(-d --dir)'{-d,--dir}'=[Dir flag example]:dir:_files -/' \
        '(-f --file)'{-f,--file}'=[File flag example]:file:_files' \
        --ours'[When checking out paths from the index, check out stage #2 for unmerged paths.]' \
        '(-t --track)'{-t,--track}'=[When creating a new branch, set up "upstream" configuration]:ref:->action_ref' \
        '(-v --version)'{-v,--version}'[Print version]' \
        '(-h --help)'{-h,--help}'[Display help]' \
        '*:: :->args' && ret=0

    case $state in
        action_ref)
            local -a choices
            choices=(${(f)"$(_call_program ref git for-each-ref --format='"%(refname)"' refs/heads 2>/dev/null)"})
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
