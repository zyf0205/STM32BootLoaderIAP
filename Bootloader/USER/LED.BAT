SET PATH=D:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files (x86)\Common Files\Oracle\Java\java8path;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;d:\software\Programs\cursor\resources\app\bin;C:\Program Files (x86)\VMware\VMware Workstation\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\Tailscale\;C:\MinGW\mingw64\bin;C:\Program Files\dotnet\;D:\Program Files\Git\cmd;D:\Program Files\nodejs\;D:\Program Files\CMake\bin;D:\Windows Kits\10\Windows Performance Toolkit\;C:\Users\zyf\AppData\Local\Programs\Python\Python37\Scripts\;C:\Users\zyf\AppData\Local\Programs\Python\Python37\;C:\Users\zyf\AppData\Local\Microsoft\WindowsApps;C:\MinGW\mingw64\bin;C:\Users\zyf\.dotnet\tools;C:\Users\zyf\AppData\Roaming\npm;D:\software\Programs\cursor\resources\app\bin;d:\software\Programs\cursor\resources\app\bin;C:\Program Files (x86)\Microsoft SQL Server\160\Tools\Binn\;C:\Program Files\Microsoft SQL Server\160\Tools\Binn\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Program Files\Microsoft SQL Server\160\DTS\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Git\cmd;C:\Program Files\MySQL\MySQL Server 8.4\bin;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\NVIDIA Corporation\NVIDIA app\NvDLISR;D:\_Software\JDK\bin;C:\Users\zyf\AppData\Local\Programs\Python\Python37\Scripts\;C:\Users\zyf\AppData\Local\Programs\Python\Python37\;C:\Users\zyf\AppData\Local\Microsoft\WindowsApps;C:\Users\zyf\AppData\Local\Programs\Microsoft VS Code\bin;C:\MinGW\mingw64\bin;C:\Users\zyf\.dotnet\tools;C:\Users\zyf\AppData\Roaming\npm;D:\software\Programs\cursor\resources\app\bin
SET CPU_TYPE=STM32F411CEUx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=LED
SET CPU_CLOCK=0x00B71B00
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\main.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_it.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\system_stm32f4xx.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\led.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\delay.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\sys.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\usart.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\obj\startup_stm32f40_41xxx._ia"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\misc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_gpio.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_rcc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_syscfg.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f4xx_usart.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\OBJ\LED.lnp"
"D:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" "..\OBJ\LED.axf" --i32combined --output "..\OBJ\LED.hex"
fromelf --bin ..\OBJ\LED.axf --output ..\OBJ\LED.bin
