langs=("c")
days=(1)
expected_results=(55208 54578)

for lang in ${langs[@]}; do
  cd "$PWD/$lang"
  for day in ${days[@]}; do
    index=$(((day - 1) * 2))
    for (( p = 1; p <= 2; p++ )); do
        name="day$day"
        out=$(eval "./test.sh $day $p")
        expected=${expected_results[$((index + p - 1))]}
        if [ "$out" = "$expected" ]; then
            echo "test passed for $name: expected $expected"
        else
            echo "test failed for $name: expected $expected but got $out"
        fi
    done
  done
done