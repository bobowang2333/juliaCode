#!/bin/bash
 
# Declare an array of string with type
declare -a StringArray=("Linux" "test" "Sec")
 
# Iterate the string array using for loop
for val in ${StringArray[@]}; do
    echo $val
done

if [ "${#StringArray[@]}" -ne 0 ]; then
    echo 'array is not empty'
    # bobo: StringArray[@]:i:j ==> i: the starting index; j: the number of elements to extract
    echo "${StringArray[@]:1:2}"
fi

echo "////// test while /////////"

while [ "${#StringArray[@]}" -ne 0 ] 
do
    # bobo: Dont add $ when declaring a new variable
    new_array=()
    delete="${StringArray[@]:0:1}"
    echo $delete
    for value in "${StringArray[@]}"
    do
        [[ $value != $delete ]] && new_array+=($value)
    done
    StringArray=("${new_array[@]}")
    unset new_array
    #StringArray=( "${StringArray[@]/$delete}" )
done

ARRAY=( "cow:moo"
        "dinosaur:roar"
        "bird:chirp"
        "bash:rock" )

for animal in "${ARRAY[@]}" ; do
    KEY=${animal%%:*}
    VALUE=${animal#*:}
    printf "%s likes to %s.\n" "$KEY" "$VALUE"
done

echo -e "${ARRAY[1]%%:*} is an extinct animal which likes to ${ARRAY[1]#*:}\n"