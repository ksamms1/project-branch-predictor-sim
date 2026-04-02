#!/bin/bash
echo "Cleaning project..."
make clean

echo "Building project..."
make

echo ""
echo "Running Smith tests..."
./sim smith 3 traces/gcc_trace.txt > out_smith_1.txt
diff -iw out_smith_1.txt validation/val_smith_1.txt

./sim smith 1 traces/jpeg_trace.txt > out_smith_2.txt
diff -iw out_smith_2.txt validation/val_smith_2.txt

./sim smith 4 traces/perl_trace.txt > out_smith_3.txt
diff -iw out_smith_3.txt validation/val_smith_3.txt

echo ""
echo "Running Bimodal tests..."
./sim bimodal 6 traces/gcc_trace.txt > out_bimodal_1.txt
diff -iw out_bimodal_1.txt validation/val_bimodal_1.txt

./sim bimodal 12 traces/gcc_trace.txt > out_bimodal_2.txt
diff -iw out_bimodal_2.txt validation/val_bimodal_2.txt

./sim bimodal 4 traces/jpeg_trace.txt > out_bimodal_3.txt
diff -iw out_bimodal_3.txt validation/val_bimodal_3.txt

echo ""
echo "Running Gshare tests..."
./sim gshare 9 3 traces/gcc_trace.txt > out_gshare_1.txt
diff -iw out_gshare_1.txt validation/val_gshare_1.txt

./sim gshare 14 8 traces/gcc_trace.txt > out_gshare_2.txt
diff -iw out_gshare_2.txt validation/val_gshare_2.txt

./sim gshare 11 5 traces/jpeg_trace.txt > out_gshare_3.txt
diff -iw out_gshare_3.txt validation/val_gshare_3.txt

echo ""
echo "Running Hybrid test..."
./sim hybrid 8 14 10 5 traces/gcc_trace.txt > out_hybrid_1.txt
diff -iw out_hybrid_1.txt validation/val_hybrid_1.txt

echo ""
echo "All tests completed."
