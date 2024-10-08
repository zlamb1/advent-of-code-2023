langs=("c")
days=(1 2 3 4 5 6 7 8 9 10 11)
expected_results=(
  55208      54578 
  2632       69629 
  537832     81939900
  23673      12263631
  910845529  77435348
  393120     36872656
  249204891  249666369
  16409      11795205644011
  1743490457 1053
  6882       491
  10165598   678728808158
)

for lang in ${langs[@]}; do
  echo "testing $lang"
  cd "$PWD/$lang"
  for day in ${days[@]}; do
    index=$(((day - 1) * 2))
    for (( p = 1; p <= 2; p++ )); do
        name="day${day}_$p"
        out=$(eval "./test.sh $day $p")
        expected=${expected_results[$((index + p - 1))]}
        if [ "$out" = "$expected" ]; then
            echo "test passed for $name: expected $expected"
        else
            echo "test failed for $name: expected $expected but got $out"
            exit 1
        fi
    done
  done
done

exit 0