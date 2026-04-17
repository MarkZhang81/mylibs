CC := gcc
LD := gcc
CFLAGS := -Wall -g

#CFLAGS := -Wall -g -I/markdev/rdma-core.upstream/build/include/
#LD_FLAGS := -L /markdev/rdma-core.upstream/build/lib

LIBS := -lrdmacm -libverbs -lmlx5 -lpthread
HEADERS := mlx5_ifc.h

all: query_hca_caps

query_hca_caps: query_hca_caps.o
	$(LD) $(LD_FLAGS) -o $@ $^ $(LIBS)

%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o query_hca_caps
