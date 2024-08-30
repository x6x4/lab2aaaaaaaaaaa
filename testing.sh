directory="tests/"

for filename in "$directory"/*; do
  if [[ -f "$filename" ]]; then
    echo "$filename:"
    valgrind build/a.out $filename
  fi
done
