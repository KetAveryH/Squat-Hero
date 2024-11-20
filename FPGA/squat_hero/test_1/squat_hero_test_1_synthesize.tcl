if {[catch {

# define run engine funtion
source [file join {C:/lscc/radiant/2024.1} scripts tcl flow run_engine.tcl]
# define global variables
global para
set para(gui_mode) "1"
set para(prj_dir) "C:/Users/KetHollingsworth/Desktop/Repositories/Microps/Squat-Hero/FPGA/squat_hero"
# synthesize IPs
# synthesize VMs
# propgate constraints
file delete -force -- squat_hero_test_1_cpe.ldc
::radiant::runengine::run_engine_newmsg cpe -syn lse -f "squat_hero_test_1.cprj" "my_pll.cprj" -a "iCE40UP"  -o squat_hero_test_1_cpe.ldc
# synthesize top design
file delete -force -- squat_hero_test_1.vm squat_hero_test_1.ldc
::radiant::runengine::run_engine_newmsg synthesis -f "squat_hero_test_1_lattice.synproj" -logfile "squat_hero_test_1_lattice.srp"
::radiant::runengine::run_postsyn [list -a iCE40UP -p iCE40UP5K -t SG48 -sp High-Performance_1.2V -oc Industrial -top -w -o squat_hero_test_1_syn.udb squat_hero_test_1.vm] [list C:/Users/KetHollingsworth/Desktop/Repositories/Microps/Squat-Hero/FPGA/squat_hero/test_1/squat_hero_test_1.ldc]

} out]} {
   ::radiant::runengine::runtime_log $out
   exit 1
}
