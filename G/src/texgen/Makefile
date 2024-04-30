SRC =  test.cc noise.cc texture.cc picoc_texture_binding.cc
OBJS = ${SRC:.cc=.o}
CC = g++
LIB=../../../tools/picoc/picoc.a ../std.a

FLAGS = -O3 -g -I..
CFLAGS = $(FLAGS) -DLINUX -DNO_MSYS -fpermissive

all: texgen.a texgen

clean:
	rm -f *.o

texgen: $(OBJS) $(LIB)
	$(CC) main.cc $(FLAGS) -DNDEBUG $(OBJS) $(LIB) -o $@

texgen.a: $(OBJS)
	ar rcs texgen.a $(OBJS)

.cc.o:
	${CC} ${CFLAGS} -DNO_MSYS -o $@ -c $<

