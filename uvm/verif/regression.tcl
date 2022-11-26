for {set i 0} {$i < 20} {incr i} {
    set db_name "covdb_$i" ;
    set xsim_command "set_property -name \{xsim.simulate.xsim.more_options\} -value \{-testplusarg UVM_TESTNAME=test_simple -testplusarg UVM_VERBOSITY=UVM_MEDIUM -sv_seed random -runall -cov_db_name $db_name\} -objects \[get_filesets sim_1\]"
    eval $xsim_command
    launch_simulation
    run all
    if {$i+1 < 20} {
        close_sim
    }
}
