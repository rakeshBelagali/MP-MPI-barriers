opemMP

cd to openMP folder

Build:
	make

clean:
	make clean

run:
	qsub ./script_comb.sh

	Note that this runs all 3 barriers (MCS, sense and built-in) at once. Output of three barriers can be seen in a single output file one after the other. They are clearly mentioned so will not create any confusion.  