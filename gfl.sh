#!/bin/bash

function init {
    ARGS=$(getopt -o ht:d:rs:p:g --long help --long type: --long depth:\
                                 --long reverse --long sort-by: --long output:\
                                 --long group --long debug: -n "$0" -- "$@")
    if [ $? != 0 ]; then
        usage
        exit 1
    fi
    eval set -- "$ARGS"

    while true ; do
        case "$1" in
            -h|--help) : ${HELP:=1}; shift;;
            -t|--type) FTYPE=$FTYPE' '$2; shift 2;;
            -d|--depth) SRCHDEP=$2; shift 2;;
            -r|--reverse) : ${REV:=1}; shift;;
            -s|--sort-by) SORTBY=$2; shift 2;;
            -p|--output) OUTPUT=$2; shift 2;;
            -g|--group) : ${GROUP:=1}; shift;;
            --debug) DEBUG=$2; shift 2;;
            --) shift; break;;
            *) echo "Internal error."; exit 1;;
        esac
    done

    if [ $HELP ]; then
        usage 1
        exit 0
    fi

    #: ${HELP:=0}
    : ${FTYPE:='reg'}
    #: ${SRCHDEP:=0}
    : ${REV:=0}
    : ${SORTBY:='path'}
    : ${OUTPUT:='rel'}
    #: ${GROUP:=0}
    : ${DEBUG:=0}

    debug 1 "HELP = $HELP"

    local declare mvalue=$FTYPE
    for value in $mvalue; do
        check_option_value '-t/--type' $value 'reg dir sym'
    done
    debug 1 "FTYPE = $FTYPE"

    debug 1 "SRCHDEP = $SRCHDEP"
    debug 1 "REV = $REV"

    check_option_value '-s/--sort-by' $SORTBY 'path file size date null'
    debug 1 "SORTBY = $SORTBY"

    check_option_value '-o/--output' $OUTPUT 'abs rel bsn'
    debug 1 "OUTPUT = $OUTPUT"
    debug 1 "GROUP = $GROUP"
    debug 1 "DEBUG = $DEBUG"

    paths[1]="$1"
    for ((i=1; i<=${#@}; i++)); do
        paths[$i]=$(eval echo -n \$$i)
    done

    : ${paths[1]:=$(pwd)}
}

function usage {
    cat <<EOF
Usage:
    gfl.sh [options] [path ...]

      Options:
      -h, --help
      -t {reg,dir,sym}, --type {reg,dir,sym}
      -d DEPTH, --depth DEPTH
      -r, --reverse
      -s {path,file,size,date}, --sort-by {path,file,size,date,null}
      -p {abs,rel,bsn}, --output {abs,rel,bsn}
      -g, --group
      --debug DEBUG-LEVEL
      -v, --version
EOF

    if [[ $# > 0 ]]; then
cat <<EOF

Options:
    -h, --help
            Show this help message and exit.

    -t {reg, dir, sym}, --type {reg, dir, sym}
            Specify the file type of the searching.

            {reg}   Regular file.
            {dir}   Directory.
            {sym}   Symbolic link.

    -d DEPTH, --depth DEPTH
            Specify the depth of the searching.

    -r, --reverse
            Reverse the result.

    -s {path,file,size,date}, --sort-by {path,file,size,date,null}
            Specify the key of sort.

            {path}  Sort by path name.
            {file}  Sort by file name.
            {size}  Sort by size of file.
            {date}  Sort by time of last modification.
            {null}  Use default order.

    -p {abs,rel,bsn}, --optput {abs,rel,bsn}
            Specify the key of sort.

            {abs}   Absolute path.
            {rel}   Relative path.
            {bsn}   File name only.

    -g, --group
            Frouping the result.

    --debug DEBUG-LEVEL
            Set the debug level.

    -v, --version
            Display the version of this program and exit.
EOF
    fi
}

function check_option_value {
    local option=$1
    local value=$2
    local declare values=$3

    local inv=1
    for cv in $values; do
        if [[ $cv == $value ]]; then
            inv=
            break
        fi
    done

    if [ $inv ]; then
        echo "Value \"$value\" invalid for option $option (choose $values)"
        usage
        exit 1
    fi
}

function debug {
    if [[ $# > 0 ]]; then
        local debug_level=$1
        shift
    fi
    : ${debug_level:=1}

    if [ $debug_level -le $DEBUG ]; then
        local print_time=$(date +%H:%M:%S.%N | sed 's/[[:digit:]]\{6\}$//')
        echo "[DebugMessage $print_time $debug_level] $@"
    fi
}

init $@

for path in ${paths[*]}; do
    debug 1 "path = $path"

    find_cmd="find $path"
    if [ $SRCHDEP ]; then
        find_cmd=$filelist" -maxdepth $SRCHDEP"
    fi

    filelist=
    declare ftype=$FTYPE
    for ft in $ftype; do
        if [ $ft = 'reg' ]; then
            filelist=$filelist$(eval $find_cmd -type f)
        elif [ $ft = 'dir' ]; then
            filelist=$filelist$(eval $find_cmd -type d)
        elif [ $ft = 'sym' ]; then
            filelist=$filelist$(eval $find_cmd -type l)
        fi
    done

    OLD_IFS=$IFS
    IFS=$(echo -en "\n\b")

    declare -a files
    file_count=0
    for file in $filelist; do
        debug 4 "file = $file"
        files[$((file_count++))]=$file
    done

    IFS=$OLD_IFS

    if [ $GROUP ]; then
        if [ $frist_g ]; then
            echo
        fi
        echo $path
        frist_g=1
    fi

    debug 2 "file total : ${#files[*]}"

    if [ $SORTBY = 'path' ]; then
        for ((j=1; j<${#files[*]}; j++)); do

            key=${files[$j]}
            i=$((j - 1))
            while [ $i \> -1 ] && [ "${files[$i]}" \> "${key}" ]; do
                files[$((i + 1))]=${files[$i]};
                ((i--));
            done
            files[$((i + 1))]=$key
        done
    elif [ $SORTBY = 'file' ]; then
        for ((j=1; j<${#files[*]}; j++)); do

            key=${files[$j]}
            i=$((j - 1))
            con_key=$(basename "$key")
            while [ $i \> -1 ] && [ "$(basename "${files[$i]}")" \> "${con_key}" ]; do
                files[$((i + 1))]=${files[$i]};
                ((i--));
            done
            files[$((i + 1))]=$key
        done
    elif [ $SORTBY = 'size' ]; then
        for ((j=1; j<${#files[*]}; j++)); do

            key=${files[$j]}
            i=$((j - 1))
            con_key=$(cat "$key" 2>/dev/null | wc -c)
            while [ $i \> -1 ] && [ $(cat "${files[$i]}" 2>/dev/null | wc -c) -gt ${con_key} ]; do
                files[$((i + 1))]=${files[$i]};
                ((i--));
            done
            files[$((i + 1))]=$key
        done
    elif [ $SORTBY = 'date' ]; then
        for ((j=1; j<${#files[*]}; j++)); do

            key=${files[$j]}
            i=$((j - 1))
            con_key=$(date +%Y%m%d%H%M%S -r "$key")
            while [ $i \> -1 ] && [ $(date +%Y%m%d%H%M%S -r "${files[$i]}") -gt ${con_key} ]; do
                files[$((i + 1))]=${files[$i]};
                ((i--));
            done
            files[$((i + 1))]=$key
        done
    fi

    for ((i=0; i<${#files[*]}; i++)); do
        echo ${files[$i]}
    done
done
