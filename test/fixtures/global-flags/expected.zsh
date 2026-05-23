#compdef mytool

# This script was generated automatically

function _mytool_backup_generate() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '(-o --to-stdout)'-o'[Prints script]' \
        '(-o --to-stdout)'--to-stdout'[Prints script]' \
        '(-h --help)'-h'[Display help]' \
        '(-h --help)'--help'[Display help]' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_backup_remote_list() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '(-o --to-stdout)'-o'[Prints script]' \
        '(-o --to-stdout)'--to-stdout'[Prints script]' \
        '(-h --help)'-h'[Display help]' \
        '(-h --help)'--help'[Display help]' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_backup_remote_unwrap() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '(-l --latest)'-l'[Fetches latest script]' \
        '(-l --latest)'--latest'[Fetches latest script]' \
        '(-o --to-stdout)'-o'[Prints script]' \
        '(-o --to-stdout)'--to-stdout'[Prints script]' \
        '(-h --help)'-h'[Display help]' \
        '(-h --help)'--help'[Display help]' \
        '1:The backup file name:' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_backup_remote() {
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
                'list:help for list'
                'unwrap:help for list'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                list)
                    _mytool_backup_remote_list "$@" && ret=0
                    ;;
                unwrap)
                    _mytool_backup_remote_unwrap "$@" && ret=0
                    ;;
            esac
            ;;
    esac

    return ret
}

function _mytool_backup() {
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
                'generate:help for generate'
                'remote:help for remote'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                generate)
                    _mytool_backup_generate "$@" && ret=0
                    ;;
                remote)
                    _mytool_backup_remote "$@" && ret=0
                    ;;
            esac
            ;;
    esac

    return ret
}

function _mytool_containers_list() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '(-n --no-headings)'-n'[Hide headings]' \
        '(-n --no-headings)'--no-headings'[Hide headings]' \
        '(-o --to-stdout)'-o'[Prints script]' \
        '(-o --to-stdout)'--to-stdout'[Prints script]' \
        '(-h --help)'-h'[Display help]' \
        '(-h --help)'--help'[Display help]' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_containers_mount() {
    local context state state_descr line
    typeset -A opt_args
    local ret=1

    _arguments -C \
        '(-o --to-stdout)'-o'[Prints script]' \
        '(-o --to-stdout)'--to-stdout'[Prints script]' \
        '(-h --help)'-h'[Display help]' \
        '(-h --help)'--help'[Display help]' \
        '1:container image file:_files' \
        '2:destination:_files' \
        '*:: :->args' && ret=0

    return ret
}

function _mytool_containers() {
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
                'list:help for list'
                'mount:help for list'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                list)
                    _mytool_containers_list "$@" && ret=0
                    ;;
                mount)
                    _mytool_containers_mount "$@" && ret=0
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
                'backup:The action to perform'
                'containers:help for containers'
            )
            _describe 'commands' choices && ret=0
            ;;
        args)
            case $line[1] in
                backup)
                    _mytool_backup "$@" && ret=0
                    ;;
                containers)
                    _mytool_containers "$@" && ret=0
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
