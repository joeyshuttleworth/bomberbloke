; Inno Setup Script for Bomberbloke on Windows x64

[Setup]
AppName=Bomberbloke
AppVersion=v0.01 Alpha
WizardStyle=modern
DefaultDirName={autopf}\BLOKE\Bomberbloke
SourceDir=C:\Users\James\Desktop\bb
DefaultGroupName=Bomberbloke
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
; Important so we can see the image
DisableWelcomePage=no
OutputDir=C:\Users\James\Desktop\bb_installer
; Image name
WizardImageFile=bomberbloke.bmp
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "*"; DestDir: "{app}"
Source: "assets\*"; DestDir: "{app}\assets"


[Icons]
Name: "{group}\Bomberbloke"; Filename: "{app}\bomberbloke_client.exe"
