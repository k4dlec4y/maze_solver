#!/bin/bash

echo "check test"
output=$(./build/maze check mazes/maze03.txt 2>&1)
if [[ $? -eq 0 || $output != "other maze was found while marching away from fence" ]]; then
    echo "failed - check for mazes/maze03.txt should fail due to: other maze was found while marching away from fence"
    exit 1
fi

output=$(./build/maze check mazes/maze04.txt 2>&1)
if [[ $? -eq 0 || $output != "less than 2 entrances" ]]; then
    echo "failed - check for mazes/maze04.txt should fail due to: less than 2 entrances"
    exit 1
fi

output=$(./build/maze check mazes/maze05.txt 2>&1)
if [[ $? -eq 0 || $output != "at least one entrance was not placed in the fence" ]]; then
    echo "failed - check for mazes/maze05.txt should fail due to: at least one entrance was not placed in the fence"
    exit 1
fi

output=$(./build/maze check mazes/maze06.txt 2>&1)
if [[ $? -eq 0 || $output != "while marking the fence, dead end was reached, x=4, y=10" ]]; then
    echo "failed - check for mazes/maze06.txt should fail due to: while marking the fence, dead end was reached, x=4, y=10"
    exit 1
fi
echo "passed"

echo "solve test"
./build/maze solve mazes/maze00.txt mazes/maze00.txt.test
status=$?
if [[ $status -ne 0 ]]; then
    echo "program should have exited with 0, but returned with $status"
    exit 1
fi
cmp mazes/maze00.txt.test mazes/maze00_solved.txt
if [[ $? -ne 0 ]]; then
    echo "mazes/maze00.txt.test is not equal with mazes/maze00_solved.txt"
    exit 1
fi
rm mazes/maze00.txt.test

./build/maze solve mazes/maze01.txt mazes/maze01.txt.test
status=$?
if [[ $status -ne 0 ]]; then
    echo "program should have exited with 0, but returned with $status"
    exit 1
fi
cmp mazes/maze01.txt.test mazes/maze01_solved.txt
if [[ $? -ne 0 ]]; then
    echo "mazes/maze01.txt.test is not equal with mazes/maze01_solved.txt"
    exit 1
fi
rm mazes/maze01.txt.test

./build/maze solve mazes/maze02.txt mazes/maze02.txt.test
status=$?
if [[ $status -ne 0 ]]; then
    echo "program should have exited with 0, but returned with $status"
    exit 1
fi
cmp mazes/maze02.txt.test mazes/maze02_solved.txt
if [[ $? -ne 0 ]]; then
    echo "mazes/maze02.txt.test is not equal with mazes/maze02_solved.txt"
    exit 1
fi
rm mazes/maze02.txt.test
echo "passed"
