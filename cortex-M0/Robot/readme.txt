## Building the Robot

### Makefile

* Since our toolchain binaries and libraries are located in different places, I have added command-line arguments to the Makefile to handle these paths.
* The Makefile contains an example of its usage in the first line.

---

### NRFX Submodule

* I had to make some adaptations in the `nrfx` submodule. These changes were also required for the drone project.
* I am not sure if these adaptations will be required for you to run the code.
* Please apply the adaptations yourself if you encounter compilation issues, as I cannot push changes directly to the `nrfx` submodule repository.
* The corresponding `git diff` for the required changes can be found below.


---

diff --git a/hal/nrf_common.h b/hal/nrf_common.h
index 2ed1c61..da6a332 100644
--- a/hal/nrf_common.h
+++ b/hal/nrf_common.h
@@ -99,13 +99,13 @@ extern "C" {
 #define NRF_CLZ(value) __builtin_clz(value)
 #endif
 
-#if defined(NRFX_CTZ)
-#define NRF_CTZ(value) NRFX_CTZ(value)
-#elif defined(ISA_ARM)
-#define NRF_CTZ(value) __CLZ(__RBIT(value))
-#else
+//#if defined(NRFX_CTZ)
+//#define NRF_CTZ(value) NRFX_CTZ(value)
+//#elif defined(ISA_ARM)
+//#define NRF_CTZ(value) __CLZ(__RBIT(value))
+//#else
 #define NRF_CTZ(value) __builtin_ctz(value)
-#endif
+//#endif
 
 #if defined(HALTIUM_XXAA) || defined(LUMOS_XXAA)
 #define DMA_BUFFER_UNIFIED_BYTE_ACCESS 1
diff --git a/mdk/nrf_common.ld b/mdk/nrf_common.ld
index ba70548..1be9298 100644
--- a/mdk/nrf_common.ld
+++ b/mdk/nrf_common.ld
@@ -414,9 +414,10 @@ SECTIONS
     __stack_end = ORIGIN(RAM) + LENGTH(RAM);
 
     __StackTop = ORIGIN(RAM) + LENGTH(RAM);
-    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
+    /* __StackLimit = __StackTop - SIZEOF(.stack_dummy); */
+    __StackLimit = __StackTop - 0x800;
     PROVIDE(__stack = __StackTop);
-
+    
     /* Check if data + heap + stack exceeds RAM limit */
     ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
 
---





