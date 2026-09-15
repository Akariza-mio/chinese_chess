param(
    [string]$MsBuildPath = "D:\VS\VS_body\MSBuild\Current\Bin\amd64\MSBuild.exe"
)

$ErrorActionPreference = "Stop"
$projectRoot = $PSScriptRoot
$projectFile = Join-Path $projectRoot "chess.vcxproj"
$releaseDirectory = Join-Path $projectRoot "x64\Release"
$distRoot = Join-Path $projectRoot "dist"
$packageName = "ChineseChess-Windows-x64"
$packageDirectory = Join-Path $distRoot $packageName
$zipPath = Join-Path $distRoot ($packageName + ".zip")

if (-not (Test-Path -LiteralPath $MsBuildPath -PathType Leaf)) {
    throw "MSBuild was not found. Pass its full path with -MsBuildPath."
}

& $MsBuildPath $projectFile /p:Configuration=Release /p:Platform=x64 /m /nologo
if ($LASTEXITCODE -ne 0) {
    throw "Release build failed with exit code $LASTEXITCODE."
}

if (Test-Path -LiteralPath $packageDirectory) {
    Remove-Item -LiteralPath $packageDirectory -Recurse -Force
}
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}

New-Item -ItemType Directory -Path $packageDirectory -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $releaseDirectory "chess.exe") `
    -Destination (Join-Path $packageDirectory "ChineseChess.exe")
Copy-Item -LiteralPath (Join-Path $releaseDirectory "raylib.dll") `
    -Destination $packageDirectory
Copy-Item -LiteralPath (Join-Path $projectRoot "assets") `
    -Destination $packageDirectory -Recurse
Copy-Item -LiteralPath (Join-Path $projectRoot "third_party\raylib\LICENSE") `
    -Destination (Join-Path $packageDirectory "raylib-LICENSE.txt")

Copy-Item -LiteralPath (Join-Path $projectRoot "release_readme.txt") `
    -Destination (Join-Path $packageDirectory "README.txt")

Compress-Archive -LiteralPath $packageDirectory -DestinationPath $zipPath
Write-Host "Package created: $zipPath"
