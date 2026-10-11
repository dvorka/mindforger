# MindForger thinking notebook
#
# Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Look up the published MindForger Windows installer on a GitHub release, download
# it, compute its SHA256, and generate the Chocolatey package sources (nuspec and
# install/uninstall scripts) ready for 'choco pack' and 'choco push'.
#
# The package does not embed the installer - it is downloaded from the GitHub
# release at install time and verified against the SHA256 computed here.
#
# Usage:
#   powershell.exe -NoProfile -ExecutionPolicy Bypass `
#       -File build\chocolatey\generate-from-release.ps1 -Version 2.3.0

param (
    [Parameter(Mandatory = $true)]
    [string]$Version
)

$ErrorActionPreference = "Stop"

$ScriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path (Join-Path $ScriptDir "..\..")

# the asset lookup is shared with winget - so that both channels always point
# to the very same binary
. (Join-Path $ScriptDir "..\winget\resolve-release-asset.ps1")

$GithubOrg  = "dvorka"
$GithubRepo = "mindforger"
$PackageId  = "mindforger"

# Inno Setup silent install flags - keep in sync with the InstallModes of the
# winget installer manifest in build/winget/generate-from-release.ps1
$SilentArgs = "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-"

# -- Step 1: find the Windows installer asset on the GitHub release ------------

$Asset = Resolve-MindForgerReleaseAsset `
    -Version $Version -GithubOrg $GithubOrg -GithubRepo $GithubRepo
$InstallerUrl = $Asset.browser_download_url

# -- Step 2: download and hash --------------------------------------------------

Write-Host "Downloading $InstallerUrl ..." -ForegroundColor Cyan
$TmpFile = [System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), $Asset.name)
Invoke-WebRequest -Uri $InstallerUrl -OutFile $TmpFile -UseBasicParsing
$Sha256 = (Get-FileHash $TmpFile -Algorithm SHA256).Hash
Remove-Item $TmpFile -Force
Write-Host "SHA256: $Sha256" -ForegroundColor Green

# -- Step 3: prepare output directory ------------------------------------------

$OutDir   = Join-Path $ProjectRoot "distro\chocolatey\$PackageId\$Version"
$ToolsDir = Join-Path $OutDir "tools"
New-Item -ItemType Directory -Force -Path $ToolsDir | Out-Null

# UTF-8 without BOM - consistent with the winget manifests
$Utf8NoBom = New-Object System.Text.UTF8Encoding $false

# -- Step 4: nuspec -------------------------------------------------------------

$NuspecXml = @"
<?xml version="1.0" encoding="utf-8"?>
<package xmlns="http://schemas.microsoft.com/packaging/2015/06/nuspec.xsd">
  <metadata>
    <id>$PackageId</id>
    <version>$Version</version>
    <title>MindForger</title>
    <authors>Martin Dvorak</authors>
    <owners>Martin Dvorak</owners>
    <projectUrl>https://www.mindforger.com</projectUrl>
    <iconUrl>https://cdn.jsdelivr.net/gh/$GithubOrg/$GithubRepo@master/app/resources/icons/mindforger128x128.png</iconUrl>
    <copyright>Copyright (C) 2016-$((Get-Date).Year) Martin Dvorak</copyright>
    <licenseUrl>https://github.com/$GithubOrg/$GithubRepo/blob/master/LICENSE</licenseUrl>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <projectSourceUrl>https://github.com/$GithubOrg/$GithubRepo</projectSourceUrl>
    <packageSourceUrl>https://github.com/$GithubOrg/$GithubRepo/tree/master/build/chocolatey</packageSourceUrl>
    <docsUrl>https://www.mindforger.com/docs/</docsUrl>
    <bugTrackerUrl>https://github.com/$GithubOrg/$GithubRepo/issues</bugTrackerUrl>
    <tags>markdown notes notebook knowledge-management pkm personal-wiki thinking-tool ide</tags>
    <summary>MindForger Thinking Notebook and Markdown IDE</summary>
    <description>MindForger is an open, free, and privacy-respecting thinking notebook and
Markdown IDE for personal knowledge management, note taking, and organizing
your thoughts, notes, and notebooks as plain Markdown files.</description>
    <releaseNotes>https://github.com/$GithubOrg/$GithubRepo/releases/tag/$Version</releaseNotes>
  </metadata>
  <files>
    <file src="tools\**" target="tools" />
  </files>
</package>
"@
[System.IO.File]::WriteAllText(
    (Join-Path $OutDir "$PackageId.nuspec"), $NuspecXml, $Utf8NoBom)

# -- Step 5: install script -----------------------------------------------------

# single-quoted here-strings + placeholders so that the $variables of the
# generated scripts are not expanded here
$InstallPs1 = @'
$ErrorActionPreference = 'Stop'

$packageArgs = @{
  packageName    = $env:ChocolateyPackageName
  softwareName   = 'MindForger*'
  fileType       = 'exe'
  url64bit       = '@@URL@@'
  checksum64     = '@@SHA256@@'
  checksumType64 = 'sha256'
  silentArgs     = '@@SILENT_ARGS@@'
  validExitCodes = @(0)
}

Install-ChocolateyPackage @packageArgs
'@
$InstallPs1 = $InstallPs1.
    Replace('@@URL@@', $InstallerUrl).
    Replace('@@SHA256@@', $Sha256).
    Replace('@@SILENT_ARGS@@', $SilentArgs)
[System.IO.File]::WriteAllText(
    (Join-Path $ToolsDir "chocolateyinstall.ps1"), $InstallPs1, $Utf8NoBom)

# -- Step 6: uninstall script ---------------------------------------------------

# Inno Setup registers the uninstaller under '{AppId}_is1' with DisplayName
# 'MindForger version x.y.z' - see build/windows/installer/mindforger-setup.iss
$UninstallPs1 = @'
$ErrorActionPreference = 'Stop'

$packageArgs = @{
  packageName    = $env:ChocolateyPackageName
  softwareName   = 'MindForger*'
  fileType       = 'exe'
  silentArgs     = '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART'
  validExitCodes = @(0)
}

[array]$key = Get-UninstallRegistryKey -SoftwareName $packageArgs['softwareName']

if ($key.Count -eq 1) {
  # Inno Setup quotes the uninstaller path
  $packageArgs['file'] = $key[0].UninstallString.Trim('"')
  Uninstall-ChocolateyPackage @packageArgs
} elseif ($key.Count -eq 0) {
  Write-Warning "$($packageArgs['packageName']) has already been uninstalled by other means."
} else {
  Write-Warning "$($key.Count) matches found - to prevent accidental data loss, no programs will be uninstalled."
  $key | ForEach-Object { Write-Warning "- $($_.DisplayName)" }
}
'@
[System.IO.File]::WriteAllText(
    (Join-Path $ToolsDir "chocolateyuninstall.ps1"), $UninstallPs1, $Utf8NoBom)

# -- Done -----------------------------------------------------------------------

Write-Host ""
Write-Host "Package sources written to: $OutDir" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  make distro-choco-pack VERSION=$Version"
Write-Host "  make distro-choco-test VERSION=$Version   (in Windows Sandbox or a VM)"
Write-Host "  make distro-choco-push VERSION=$Version"
