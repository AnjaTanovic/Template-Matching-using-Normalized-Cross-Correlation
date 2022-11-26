CC = "g++"
PROJECT = specification
SRC = sc_main.cpp functions.cpp soft.cpp interconnect.cpp hard.cpp utils_pkg.vhd memory_beh.vhd address_generation_rtl.vhd calculation_rtl.vhd dsp2_loop_beh.vhd dsp_loop_beh.vhd loop_structure.vhd top_structure.vhd   

$(PROJECT) : $(SRC)
	xmsc_run -sc_main +systemc_args+"./preprocessing/dimensions_2.txt ./preprocessing/image_2_0.txt ./preprocessing/image_2_1.txt ./preprocessing/image_2_2.txt ./preprocessing/template_2_0.txt ./preprocessing/template_2_1.txt ./preprocessing/template_2_2.txt" -gui $(SRC)

clean:
	rm specification *.txt



