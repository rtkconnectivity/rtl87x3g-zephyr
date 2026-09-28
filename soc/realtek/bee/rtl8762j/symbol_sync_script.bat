@REM 1>NUL copy       ..\..\..\..\..\..\rea\sdk\bin\ROM.a                      platform\rom_symbol\ROM.a
1>NUL copy       ..\..\..\..\..\..\realtek\kernel\staging\rtl8762j\inc\rom_uuid.h                   platform\inc\


@echo %TIME%
start python    ..\..\..\..\..\..\realtek\tools\script_internal\KeilToGCC\keil_lib_to_gcc_lib.py ..\..\..\..\..\..\realtek\bin\rtl8762j\rom_lib\ROM.lib    platform\rom_symbol\rom.a
@REM start python    ..\..\..\..\..\..\bb2ultra-dev\sdk\tool\gcc_tool\arcclib_to_gcclib.py ..\..\..\..\..\..\bb2ultra-dev\keil_proj\lib\upperstack.lib    ..\..\..\..\..\realtek-app\subsys\bluetooth\bt_host\lib\upperstack.a
