if {[catch {

# define run engine funtion
source [file join {C:/lscc/radiant/2024.1} scripts tcl flow run_engine.tcl]
# define global variables
global para
set para(gui_mode) "1"
set para(prj_dir) "C:/Users/KetHollingsworth/Desktop/Repositories/Microps/Squat-Hero/FPGA/SPI_FPGA"
# synthesize IPs
# synthesize VMs
# synthesize top design
file delete -force -- SPI_FPGA_SPI.vm SPI_FPGA_SPI.ldc
::radiant::runengine::run_engine_newmsg synthesis -f "SPI_FPGA_SPI_lattice.synproj" -logfile "SPI_FPGA_SPI_lattice.srp"
::radiant::runengine::run_postsyn [list -a iCE40UP -p iCE40UP5K -t SG48 -sp High-Performance_1.2V -oc Industrial -top -w -o SPI_FPGA_SPI_syn.udb SPI_FPGA_SPI.vm] [list C:/Users/KetHollingsworth/Desktop/Repositories/Microps/Squat-Hero/FPGA/SPI_FPGA/SPI/SPI_FPGA_SPI.ldc]

} out]} {
   ::radiant::runengine::runtime_log $out
   exit 1
}
