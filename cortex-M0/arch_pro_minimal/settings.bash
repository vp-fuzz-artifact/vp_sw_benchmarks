## Required
# Path to harness
export TC_PATH="$(pwd)/vp_mode/harness/build/harness"
# Path vp
export TC_VP_EXECUTABLE="$(pwd)/vp_mode/VPs/avp32/install/bin/avp64-runner"
# Launch arguments of vp
export TC_VP_LAUNCH_ARGS="-f"
# Add required libraries
#export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/vp_mode/avp64/install/lib/:$(pwd)/vp_mode/avp64/install/lib64/"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/vp_mode/VPs/avp32/install/lib/:$(pwd)/vp_mode/VPs/avp32/install/lib64/"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/vp_mode/VPs/avp32/build/ocx-qemu-arm-avp64-src/unicorn/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/vp_mode/VPs/avp32/build/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/vp_mode/vp-testing-interface/build


## Optional
# 0: No logging (default), 1: Everything, 2: Errors only
export TC_LOGGING="1"
export TC_LOGGING_PATH="h_out.txt"
# 0: No logging (default), 1: Everything, 2: Errors only
export TC_VP_LOGGING="1"
export TC_VP_LOGGING_PATH="vp_out.txt"
# 0: Not killing (default), 1: Killing old instances
export TC_KILL_OLD="1"

# Mode
# 0: Restart after each run, 1: Persistent mode
export TC_MODE="1"
# Number of the VP process instances used. Required for TC_MODE=0.
export TC_VP_INSTANCES="1"
# Fuzzing Settings
export TC_START_SYMBOL="main"
export TC_END_SYMBOL="exit"
export TC_RETURN_REGISTER="r0"
#export TC_MMIO_DATA_ADDRESS="0x40002518"
#export TC_MMIO_RDY_ADDRESS="0x40002518"

## Additional for AFLplusplus
export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES="1"
export AFL_SKIP_CPUFREQ="1"

## License for usima
export LMX_LICENSE_PATH="6200@lic-machineware.ice.rwth-aachen.de"

#Additional for setup
#export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/net/sw/gcc/gcc-11.4.1/lib64"
