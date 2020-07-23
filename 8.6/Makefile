EXECS=mvm
MPICC?=mpicc

all: ${EXECS}

mvm: mvm.c
		${MPICC} -O -o mvm mvm.c

clean:
	rm -f ${EXECS}
