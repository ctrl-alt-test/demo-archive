@echo off

rem Affiche les lignes contenant des floats susceptibles de prendre plus de 2 octets

..\tools\sed -n "s_//.*__; s/\.\(0\|5\|25\|75\)f//g; /[0-9]\+\.[0-9]\+/p" src/*.cc data/textures/*.cc | ..\tools\sed /\".*\",/d
