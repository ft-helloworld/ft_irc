#!/bin/zsh

batch() {
    for arg; do
        /Users/smun/ft_irc/tests/dummyclientserver/dummyclient "$arg" &
    done
    wait
}

batch b{1..500}
