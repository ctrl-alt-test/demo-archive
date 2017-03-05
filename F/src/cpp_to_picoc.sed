# suppression du prélude
/TEXTURE_NAMES/,/.*TEXTURE_BUILDER_BODY.*/d

# suppression du type ": unsigned char" des enums
/enum/s/:.*//

# suppresion du namespace
/namespace/d

# suppression des macros
/^#/d

# constructeur par copie
s/Channel *\([A-Za-z0-9]\+\) = \([A-Za-z0-9]\+\)/struct Texture* \1 = New("\1"); Copy(\1, \2)/g

# constructeur par défaut
s/Channel *\([A-Za-z0-9]\+\)/struct Texture* \1 = New("\1")/g

# suppression du protoype de la fonction à appeler
/^void *build/d

# min/max
s/\.MinimizeTo/.Min/g
s/\.MaximizeTo/.Max/g

# floats : .3f -> 2.3
s/\([0-9]\.[0-9]*\)f/\1/g

# OO sans argument
s/\([A-Za-z0-9]\+\)\.\([A-Za-z0-9]\+\)()/\2(\1)/g
#te

# OO avec argument
s/\([A-Za-z0-9]\+\)\.\([A-Za-z0-9]\+\)(/\2(\1, /g

#:e
# opérateurs += *= et -=
s/\([A-Za-z0-9]\+\) *+= *\([A-Za-z0-9]\+\)/Add(\1, \2)/g
s/\([A-Za-z0-9]\+\) *\*= *\([A-Za-z0-9]\+\)/MultT(\1, \2)/g
s/\([A-Za-z0-9]\+\) *\-= *\([A-Za-z0-9]\+\)/Sub(\1, \2)/g
