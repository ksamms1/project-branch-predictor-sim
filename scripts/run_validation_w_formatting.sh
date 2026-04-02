#!/bin/bash
echo "Cleaning project..."
make clean

echo "Building project..."
make

echo ""
echo "Running Smith tests..."
echo ""

./sim smith 3 traces/gcc_trace.txt | head -n 7; echo
head -n 7 validation/val_smith_1.txt; echo
./sim smith 3 traces/gcc_trace.txt > out_smith_1.txt
diff -iw out_smith_1.txt validation/val_smith_1.txt && echo "" && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
./sim smith 1 traces/jpeg_trace.txt | head -n 7; echo
head -n 7 validation/val_smith_2.txt; echo
./sim smith 1 traces/jpeg_trace.txt > out_smith_2.txt
diff -iw out_smith_2.txt validation/val_smith_2.txt && echo "" && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
./sim smith 4 traces/perl_trace.txt | head -n 7; echo
head -n 7 validation/val_smith_3.txt; echo
./sim smith 4 traces/perl_trace.txt > out_smith_3.txt
diff -iw out_smith_3.txt validation/val_smith_3.txt && echo "" && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
echo ""
echo "Running Bimodal tests..."
echo ""
./sim bimodal 6 traces/gcc_trace.txt | head -n 7; echo
head -n 7 validation/val_bimodal_1.txt; echo
./sim bimodal 6 traces/gcc_trace.txt > out_bimodal_1.txt
diff -iw out_bimodal_1.txt validation/val_bimodal_1.txt && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
./sim bimodal 12 traces/gcc_trace.txt | head -n 7; echo
head -n 7 validation/val_bimodal_2.txt; echo
./sim bimodal 12 traces/gcc_trace.txt > out_bimodal_2.txt
diff -iw out_bimodal_2.txt validation/val_bimodal_2.txt && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
./sim bimodal 4 traces/jpeg_trace.txt | head -n 7; echo
head -n 7 validation/val_bimodal_3.txt; echo
./sim bimodal 4 traces/jpeg_trace.txt > out_bimodal_3.txt
diff -iw out_bimodal_3.txt validation/val_bimodal_3.txt && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
echo ""
echo "Running Gshare tests..."
echo ""
./sim gshare 9 3 traces/gcc_trace.txt | head -n 7; echo
head -n 7 validation/val_gshare_1.txt; echo
./sim gshare 9 3 traces/gcc_trace.txt > out_gshare_1.txt
diff -iw out_gshare_1.txt validation/val_gshare_1.txt && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
./sim gshare 14 8 traces/gcc_trace.txt | head -n 7; echo
head -n 7 validation/val_gshare_2.txt; echo
./sim gshare 14 8 traces/gcc_trace.txt > out_gshare_2.txt
diff -iw out_gshare_2.txt validation/val_gshare_2.txt && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
./sim gshare 11 5 traces/jpeg_trace.txt | head -n 7; echo
head -n 7 validation/val_gshare_3.txt; echo
./sim gshare 11 5 traces/jpeg_trace.txt > out_gshare_3.txt
diff -iw out_gshare_3.txt validation/val_gshare_3.txt && echo ">>> SUCCESS: No differences found."
echo ""
read -p "Press [Enter] to continue..."
echo ""
echo ""
echo "Running Hybrid test..."
echo ""
./sim hybrid 8 14 10 5 traces/gcc_trace.txt | head -n 7; echo
head -n 7 validation/val_hybrid_1.txt; echo
./sim hybrid 8 14 10 5 traces/gcc_trace.txt > out_hybrid_1.txt
diff -iw out_hybrid_1.txt validation/val_hybrid_1.txt && echo ">>> SUCCESS: No differences found."

echo ""
echo "All tests completed."
