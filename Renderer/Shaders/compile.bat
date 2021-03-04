%VK_SDK_PATH%\bin\dxc.exe -spirv -fspv-target-env=vulkan1.2 -T cs_6_6 -E main plain_color.hlsl -Fo plain_color.spv
%VK_SDK_PATH%\bin\spirv-val.exe plain_color.spv
pause