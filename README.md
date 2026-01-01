# Benchmark Examples for the Virtual Platform Testing Interface with AVP64

This folder contains the executables and configuration files needed to run each benchmark with our fuzzing tool.
Each benchmark folder contains (at least) the following files and  subfolders:
1. A build directory with the executable (.bin and .elf) or the sdcard image, in case we're running Linux
2. The seeds directory, which contains the seeds to give in input to AFL++
3. The mmio_config.json, which defines actions to take when the target program accesses a specific address
4. The VP configuration file (.cfg) that defines the VP configuration, e.g., if the VP should or not wait for a gdb connection, the address of the components, and so forth
5. The script to launch the fuzzing (`run_fuzzer.bash`)
6. The script that configures the interface, the software between the fuzzer and the VP (`settings.bash`). This script defines, e.g., which checkpointing technique to use, the path to the VP executable etc

| Example target             | Description |
|----------------------------|-------------|
| Arch pro                   | It reads a string via UART until a termination character and if it matches the password it will exit with a 1 otherwise with a 0. |
| zlib (zephyr, bm, Linux)   | An application that uncompress the received stream of bytes |
| crypto (zephyr, bm, Linux) | An application to test various AES mode. |

## The mmio_config.json file
This file defines what registers the VP's mmio probe should track and what action should be associated to it.
When the probe intercept the address specified in the json, it looks for an action to perform. We currently support 3 actions:
- `start_injection`: the probe spawns an sc thread using `sc_spawn()` which will read fuzzing data and send them to the corresponding peripheral at a predefined interval
- `notify_sim_end`: this is used to detect when the simulation ends. It is usually associated to the simdev peripheral, which is usually accessed by the running software at the end of the main routine. The same result can also be achieved by setting a breakpoint to a symbol (see settings.bash).
- `notify_init_end`: this detects the end of the software booting or initialization routine. The detection of this access starts the checkpointing routine.
The same result can also be achieved by setting a breakpoint to a symbol (see settings.bash).


## The settings.bash file

Below we report the list of environmental variables required by our tool to work properly.

| ENV Variable               | Required       | Description |
|----------------------------|----------------|-------------|
| `TC_PATH`                  | Required       | Path to the harness executable. Usually `vp_mode/harness/build/harness`. |
| `TC_VP_EXECUTABLE`         | Required       | Path to the VP executable. Usually `vp_mode/VPs/<VP>/install/bin/<VP>`. |
| `TC_VP_LAUNCH_ARGS`        | Optional       | Additional arguments passed to the VP. |
| `LD_LIBRARY_PATH`          | Required       | Required library paths for the VP. |
| `TC_LOGGING`               | Optional       | Harness logging level: `0 = off` (default), `1 = verbose`, `2 = errors only`. |
| `TC_LOGGING_PATH`          | Optional       | Path to harness log file. |
| `TC_VP_LOGGING`            | Optional       | VP logging level: `0 = off` (default), `1 = verbose`, `2 = errors only`, `3 = verbose (Console)`. |
| `TC_VP_LOGGING_PATH`       | Optional       | Path to VP log file. |
| `TC_CHKPT_MODE`            | Required       | Checkpointing mode: `0 = No checkpointing`, `1 = CRIU mode`, `2 = Fork mode`, `3 = Hybrid`. |
| `TC_DUMP`                  | Required       | Checkpointing mode: `0 = Read from stable checkpoint and restore only`, `1 = Dump`. |
| `TC_START_SYMBOL`          | Required       | Symbol marking the start of the fuzzed region. Can be `""`. |
| `TC_END_SYMBOL`            | Required       | Symbol marking the end of the fuzzed region. Value in `TC_RETURN_REGISTER` is read here. |
| `TC_RETURN_REGISTER`       | Required       | Register to read at `TC_END_SYMBOL` (e.g., `r0`). |
| `TC_ERROR_SYMBOL`          | Optional       | If reached, execution aborts with return code 0. Useful for exception handlers. |
| `AFL_SKIP_CPUFREQ`         | Required       | AFL++ setting to avoid CPU frequency scaling checks. |
| `AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES` | Required | AFL++ setting to ignore crash detection. |


> ℹ️ See `baremetal/arch_pro/Settings.bash` for an environment setup example.

Beside from exporting environmental variables, settings.bash also checks if CRIU was specified as a checkpointing strategy and opens a criu socket.

## The run_fuzzer.bash file

This script calls the `afl-fuzz` binary passing:
- A fixed seed with the `-s` option. This is used by the fuzzer's internal mutation engine and setting it to a constant value makes the fuzzer deterministic.
- The seed directory
- The out directory, where the fuzzer writes the results
- The timeout
- The `-v` option to set the fuzzer to work with our tool.
- The VP's configuration file







