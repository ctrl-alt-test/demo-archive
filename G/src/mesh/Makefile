SRC = mesh.cc cube.cc revolution.cc revolutionfunctions.cc meshpool.cc pave.cc
OBJS = ${SRC:.cc=.o}
CC = g++
LIB=../std.a ../texgen/texgen.a ../../../tools/picoc/picoc.a

FLAGS = -O3 -g -I.. -std=c++0x
CFLAGS = $(FLAGS) -DLINUX -DNO_MSYS -fpermissive -DDEBUG

all: meshgen.a

clean:
	rm -f *.o

meshgen: $(OBJS) $(LIB)
	$(CC) ${CFLAGS} main.cc $(FLAGS) -DNDEBUG $(OBJS) $(LIB) -o $@

meshgen.a: $(OBJS)
	ar rcs meshgen.a $(LIB) $(OBJS)

.cc.o:
	${CC} ${CFLAGS} -DNO_MSYS -o $@ -c $<

