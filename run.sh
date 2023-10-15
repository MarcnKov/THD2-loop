#!/bin/bash

make_path='VimbaC/Examples/SynchronousGrab/Build/Make'
program_path=$make_path'/binary/x86_64bit/SynchronousGrab'
executable_path=$make_path'/binary/x86_64bit/'


if [ ! -f "$program_path" ]
then
	cd "$make_path"
	make
	cd -
fi

if [ ! -f "Visualization/RX_TX_data/get_param" ] || [ ! -f "Visualization/RX_TX_data/set_param" ]
then
	chmod +x Visualization/RX_TX_data/recompile_c.sh
	./Visualization/RX_TX_data/recompile_c.sh
	mv get_param set_param Visualization/RX_TX_data
fi

# Run the programs
chmod +x "$program_path"

cd $executable_path
gnome-terminal -- ./SynchronousGrab 

cd ../../../../../../../Visualization
gnome-terminal -- python3 visualization.py
