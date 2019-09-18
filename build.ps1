& ${env:ProgramFiles(x86)}\Microsoft` Visual` Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe .\VS2019\pvr.chinachu.sln /t:Clean,Build /p:Configuration=Release /p:Platform=x86
New-Item dist -ItemType Directory
Copy-Item -r .\template\pvr.chinachu .\dist\
Copy-Item .\ChangeLog.txt .\dist\pvr.chinachu
Copy-Item .\LICENSE .\dist\pvr.chinachu
Copy-Item .\VS2019\Release\pvr.chinachu.dll .\dist\pvr.chinachu
Set-Location .\dist
Compress-Archive -Force -CompressionLevel NoCompression -Path .\pvr.chinachu -DestinationPath ..\pvr.chinachu.zip
Write-Warning "PowerShell command 'Compress-Archive' creates broken zip file.
Please unzip .\pvr.chinachu.zip yourself, and re-zip it with other compression tool."
