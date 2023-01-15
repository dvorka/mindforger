#!/bin/bash
# Screenshots are taken in 1360x768 resolution - image sizes are: 1360x741

#identify *.*

convert "${1}" -resize 1360x741\! -quality 100 "../${1}"

# eof
