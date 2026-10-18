$ErrorActionPreference = 'Stop'

$packageArgs = @{
  packageName    = $env:ChocolateyPackageName
  softwareName   = 'MindForger*'
  fileType       = 'exe'
  url64bit       = 'https://github.com/dvorka/mindforger/releases/download/2.5.0/windows-installer-mindforger-2.5.0.exe'
  checksum64     = '3CB3FC8C441578D59CAFEA43F51FAA654E40D565F137F5B5FDCC959D21568315'
  checksumType64 = 'sha256'
  silentArgs     = '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-'
  validExitCodes = @(0)
}

Install-ChocolateyPackage @packageArgs