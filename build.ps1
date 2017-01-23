& ${env:ProgramFiles(x86)}\MSBuild\14.0\Bin\MSBuild.exe .\VS2015\pvr.chinachu.sln /t:Clean,Build /p:Configuration=Release /p:Platform=x86
Copy-Item -r .\template\pvr.chinachu .\
Copy-Item .\ChangeLog.txt .\pvr.chinachu
Copy-Item .\LICENSE .\pvr.chinachu
Copy-Item .\VS2015\Release\pvr.chinachu.dll .\pvr.chinachu
Compress-Archive -Force -CompressionLevel NoCompression -Path .\pvr.chinachu -DestinationPath .\pvr.chinachu.zip
Remove-Item -Recurse  .\pvr.chinachu
Write-Warning "PowerShell command 'Compress-Archive' creates broken zip file.
Please unzip .\pvr.chinachu.zip yourself, and re-zip it with other compression tool."
