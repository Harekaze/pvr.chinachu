$version="2.0.1"
& ${env:ProgramFiles(x86)}\MSBuild\14.0\Bin\MSBuild.exe .\VS2015\pvr.chinachu.sln /t:Clean,Build /p:Configuration=Release /p:Platform=x86 /p:DefineConstants="VERSION=\`"$version\`""
Copy-Item -r .\template\pvr.chinachu .\
Copy-Item .\ChangeLog.txt .\pvr.chinachu
Copy-Item .\LICENSE .\pvr.chinachu
Get-Content .\template\pvr.chinachu\addon.xml | % { $_ -replace "VERSOION","$version" } | Set-Content .\pvr.chinachu\addon.xml
Copy-Item .\VS2015\Release\pvr.chinachu.dll .\pvr.chinachu
Compress-Archive -Force -CompressionLevel NoCompression -Path .\pvr.chinachu -DestinationPath .\pvr.chinachu.$version.zip
Remove-Item -Recurse  .\pvr.chinachu
Write-Warning "PowerShell command 'Compress-Archive' creates broken zip file.
Please unzip .\pvr.chinachu.$version.zip yourself, and re-zip it with other compression tool."
