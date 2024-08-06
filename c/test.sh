dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
name="day$1"
cd "$dir/$name"
gcc "$name.c" -lm
./a.out