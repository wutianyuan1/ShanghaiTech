# !usr/bin/bash
sudo cp mem.circ tests/circ_files
sudo cp alu.circ tests/circ_files
sudo cp regfile.circ tests/circ_files
sudo cp cpu.circ tests/circ_files
cd tests/circ_files
sudo python3 sanity_test.py
cd ../..
