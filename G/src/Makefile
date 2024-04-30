CC=g++
CFLAGS=-Wall -W -g -DUNIX_HOST -I. -DLINUX -DDEBUG

TARGET	= std.a
SRCS	= randomness.cc spline.cc algebra/matrix.cc variable.cc
OBJS	:= $(SRCS:%.cc=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $(TARGET) $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS) *~

.cc.o:
	${CC} ${CFLAGS} -DNO_MSYS -o $@ -c $<

cat.a: $(TARGET)
	make -C texgen
	make -C mesh
	/bin/echo -e "create cat.a\naddlib std.a\naddlib texgen/texgen.a\naddlib mesh/meshgen.a\nsave\nend" | ar -M

#depend:
#	$(CC) -MM $(SRCS) >.depend

#-include .depend
