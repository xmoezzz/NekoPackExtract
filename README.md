# NekoPackExtract
NekoPackExtract : A tool that can extract almost all resources used by NekoPack Engine(Chartreux Inc) and repack some of them for translation.


## How To Extract
![Main Window](https://github.com/xmoeproject/NekoPackExtract/blob/master/Image/tool.jpg)    

- Execute NekoPackExractTool.exe.  
- You may drag single / multiple files at once.
- DO NOT edit filename.

## How To Repack
- run NekoPackRebuilder.exe in command prompt with parameters: NekoPackRebuilder.exe original.dat full/path/to/repacking/directory output.dat  
- DO NOT put any other files in this directory, program will parse filename to get metadata.
- eg: NekoPackRebuilder.exe system.dat E:\NekoPackRebuilder\Release\system output.dat

## How To Repack MNG Images
- run PackMNG.exe in command prompt with parameters: PackMNG.exe original.mng full/path/to/pngs
- eg: PackMNG.exe 0.mng E:\pack

## Supported resource types
- Archive： *.dat(sig : NEKOPACK)
- Images : *.mng
