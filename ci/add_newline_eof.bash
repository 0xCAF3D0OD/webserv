#!/bin/bash

nonewline () {
	for f in "$@" ; do if [[ -f "$f" && -s "$f" && ! ( -z "$(tail -c 1 "$f")" ) ]] ; then echo "" >> "$f" ; else echo "$f has already newline" ; fi ; done
}

nonewline $(find . -name "*.hpp" -or -name "*.cpp")
