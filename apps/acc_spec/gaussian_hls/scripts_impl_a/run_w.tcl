set ROOT [pwd]
set SRC $ROOT/verilog
set TCL $ROOT/scripts_impl_a
set EXP $ROOT/exps_impl_a

include $TCL/setup_w.tcl
include $TCL/eqcheck_w.tcl

set_engine_mode {Hp Ht B N}
set_prove_time_limit 0s; set_prove_per_property_time_limit 0s
set_max_trace_length 0; set_prove_per_property_max_time_limit 60m

assume -disable {::Subset - A}
assume -disable {::Subset - B}
assume -disable {::Subset - C1}
assume -disable {::Subset - C2}
assume -disable {::Subset - C3}
assume -disable {::Subset - C4}
assume -disable {::Subset - D}
assume -disable {::Subset - E}

#assume -from_assert {<embedded>::bound help 0} {<embedded>::bound help 1} {<embedded>::bound help 2} {<embedded>::bound help 3} {<embedded>::bound help 4} {<embedded>::bound help 5} {<embedded>::bound help 6} {<embedded>::bound help 7} {<embedded>::bound help 8} {<embedded>::bound help 9} {<embedded>::bound help 10} {<embedded>::bound help 11} {<embedded>::bound help 12} {<embedded>::bound help 13} {<embedded>::bound help 14} {<embedded>::bound help 15}

#prove -property {<embedded>::bound}

#set_engine_mode {Ht B N}
#set_prove_time_limit 0s; set_prove_per_property_time_limit 0s
#set_max_trace_length 15; set_prove_per_property_max_time_limit 0s

#prove -all

#save -jdb $EXP/wri_all.jdb -capture_setup -capture_session_data

#exit

#assume -enable {::Subset - A}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_A.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_A.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - A}

#assume -enable {::Subset - C3}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_C3.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_C3.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - C3}

#assume -enable {::Subset - C4}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_C4.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_C4.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - C4}

#assume -enable {::Subset - E}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_E.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_E.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - E}

#

#assume -enable {::Subset - C1}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_C1.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_C1.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - C1}

#assume -enable {::Subset - C2}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_C2.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_C2.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - C2}

#

#assume -enable {::Subset - B}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_B.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_B.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - B}

#assume -enable {::Subset - D}
#include $TCL/iterate_ctr_w.tcl
#save -jdb $EXP/wri_ctr_D.jdb -capture_setup -capture_session_data
#include $TCL/iterate_dat_w.tcl
#save -jdb $EXP/wri_dat_D.jdb -capture_setup -capture_session_data
#assume -disable {::Subset - D}

#exit

