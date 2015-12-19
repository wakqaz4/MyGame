@echo off
setlocal EnableDelayedExpansion
if %processor_architecture% == AMD64 (
	set arch=x64
	if not exist bin\main_!arch!.exe (
		set arch=x86
	) else (
		set PATH=x64;bin\x64;!PATH!;
	)
) else (
	set arch=x86
)
start bin\main_%arch%.exe  -video_app auto -video_vsync 0 -video_refresh 0 -video_multisample 0 -video_gamma 1.0 -video_mode 1 -video_resizable 0 -video_fullscreen 0 -sound_app auto -data_path "../"  -engine_config "../data/RTS_Demo1/unigine.cfg" -engine_log "log.html" -system_script "RTS_Demo1/unigine.cpp" -editor_script "editor/editor.cpp" -gui_path "core/gui/" -extern_plugin "Collada","OpenFlight","Autodesk" -console_command "config_readonly 1 && world_load \"RTS_Demo1/RTS_Demo1\""