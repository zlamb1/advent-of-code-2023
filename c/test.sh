dir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
name="day$1"
cd "$dir/$name"

for part in 1 2
do
    part="_$part"
    file_name="day$1$part.c"
    if [ -f "$file_name" ]; then
        echo "running $file_name"
        gcc "$file_name" -lm
        ./a.out
        [ "$part" = "_2" ] || echo ""
    else
        echo "could not locate $file_name"
    fi
done