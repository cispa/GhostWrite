ghostwrite: ghostwrite.c
	riscv64-linux-gnu-gcc ghostwrite.c -static -march="rv64gvzve64x" -o ghostwrite

clean:
	rm -rf ghostwrite
