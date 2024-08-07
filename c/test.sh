dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
name="day$1"
cd "$dir/$name"

run_test () {
    gcc "$1" -lm
    if [ -f "./a.out" ]; then
        ./a.out
        rm ./a.out
    fi
}

if [[ $# -eq 2 ]]; then
    if [[ $2 -lt 1 ]] || [[ $2 -gt 2 ]]; then
        echo "expected part 1 or 2"
        exit 1
    fi
    part="_$2"
    file_name="day$1$part.c"
    if [ -f "$file_name" ]; then
        run_test $file_name
    else
        echo "could not locate $file_name"
    fi
elif [[ $# -eq 1 ]]; then
    for part in 1 2
    do
        part="_$part"
        file_name="day$1$part.c"
        if [ -f "$file_name" ]; then
            run_test $file_name
        else
            echo "could not locate $file_name"
        fi
    done
else
    echo "expected day number"
    exit 1
fi

exit 0