directory="tests/"

for filename in "$directory"/*; do
  if [[ -f "$filename" ]]; then
    echo "$filename:"
    build/a.out $filename
  fi
done
