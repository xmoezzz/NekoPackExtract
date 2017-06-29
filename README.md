# NekoPackExtract
NekoPackExtract : A tool that can extract almost all resources used by NekoPack Engine(Chartreux Inc) and repack some of them for translation.


## How To Extract
![Main Window](https://github.com/xmoeproject/NekoPackExtract/blob/master/Image/tool.jpg)    

- Execute NekoPackExractTool.exe.  
- You may drag single / multiple files at once.

## How To Repack
- run NekoPackRebuilder.exe in command prompt with parameters: NekoPackRebuilder.exe original.dat full/path/to/repacking/directory output.dat  
- eg: NekoPackRebuilder.exe system.dat E:\NekoPackRebuilder\Release\system output.dat

## Supported resource types
- Archive： *.dat(sig : NEKOPACK)
- Images : *.mng
