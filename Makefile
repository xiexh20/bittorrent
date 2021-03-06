# Some variables
CC 		= gcc
CFLAGS		= -g -Wall -DDEBUG
SOS = lib/*.so	# all shared library files
LIBS = -lsha -ldebug -lmtcp -lspiffy -ldplist -lpacket -lchunk -lbt_parse
LIBPATH = -lm -L./lib -Wl,-rpath=./lib
LDFLAGS		= -lm
TESTDEFS	= -DTESTING			# comment this out to disable debugging code
OBJS		= peer.o bt_parse.o spiffy.o debug.o input_buffer.o chunk.o sha.o
MK_CHUNK_OBJS   = make_chunks.o chunk.o sha.o
COMFS = lib/libdebug.so lib/libmtcp.so lib/libsha.so lib/libspiffy.so lib/libdplist.so lib/libpacket.so lib/libchunk.so lib/libbt_parse.so

BINS            = peer make-chunks server client
TESTBINS        = test_debug test_input_buffer

# Implicit .o target
.c.o:
	$(CC) $(TESTDEFS) -c $(CFLAGS) $<

# Explit build and testing targets

all: ${BINS} ${TESTBINS}

run: peer_run
	./peer_run

test: peer_test
	./peer_test

peer: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

make-chunks: $(MK_CHUNK_OBJS)
	$(CC) $(CFLAGS) $(MK_CHUNK_OBJS) -o $@ $(LDFLAGS)

server: server.c  $(COMFS)		# makefile will search receipt for file mentioned here but not exit yet
	gcc -g server.c $(COMFS) $(LIBS) $(LIBPATH) -o server
client: client.c $(COMFS)
	gcc -g client.c $(LIBS) $(LIBPATH) -o client

test_mtcp: test_mtcp.c $(COMFS)
	gcc -g test_mtcp.c $(COMFS) $(LIBS) $(LIBPATH) -o test_mtcp

btp_test: bt_parse_test.c $(COMFS)
	gcc -g bt_parse_test.c $(COMFS) $(LIBS) $(LIBPATH) -o btp_test


# compile shared library
lib/libmtcp.so: lib/mtcp.c
	gcc lib/mtcp.c -g -o lib/libmtcp.so -lm -fPIC -shared

lib/libsha.so: lib/sha.c
	gcc lib/sha.c -g -o lib/libsha.so -lm -fPIC -shared

lib/libdebug.so: lib/debug.c
	gcc lib/debug.c -g -o lib/libdebug.so -lm -fPIC -shared

lib/libspiffy.so: lib/spiffy.c
	gcc lib/spiffy.c -g -o lib/libspiffy.so -lm -fPIC -shared

lib/libdplist.so: lib/dplist.c
	gcc lib/dplist.c -g -o lib/libdplist.so -lm -fPIC -shared

lib/libpacket.so: lib/packet.c
	gcc lib/packet.c -g -o lib/libpacket.so -lm -fPIC -shared

lib/libchunk.so: lib/chunk.c
	gcc lib/chunk.c -g -o lib/libchunk.so -lm -fPIC -shared

lib/libbt_parse.so: lib/bt_parse.c
	gcc lib/bt_parse.c -g -o lib/libbt_parse.so -lm -fPIC -shared


clean:
	rm -f *.o $(BINS) $(TESTBINS) $(SOS)

bt_parse.c: bt_parse.h

# The debugging utility code

debug-text.h: debug.h
	./debugparse.pl < debug.h > debug-text.h

test_debug.o: debug.c debug-text.h
	${CC} debug.c ${INCLUDES} ${CFLAGS} -c -D_TEST_DEBUG_ -o $@

test_input_buffer:  test_input_buffer.o input_buffer.o



