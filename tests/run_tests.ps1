$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

# gcc from MSYS may fail on Unicode paths. Use short (8.3) paths when possible.
Add-Type -Namespace Win32 -Name PathUtil -MemberDefinition @'
[System.Runtime.InteropServices.DllImport("kernel32.dll", CharSet=System.Runtime.InteropServices.CharSet.Unicode)]
public static extern int GetShortPathName(string longPath, System.Text.StringBuilder shortPath, int shortPathLength);
'@

function Get-ShortPath([string]$path) {
    $sb = New-Object System.Text.StringBuilder 260
    $len = [Win32.PathUtil]::GetShortPathName($path, $sb, $sb.Capacity)
    if ($len -gt 0) { return $sb.ToString() }
    return $null
}

$rootShort = Get-ShortPath $root
if (-not $rootShort) { $rootShort = $root }

$lab01Exe = Join-Path $root "lab01\lab01.exe"
$lab02Exe = Join-Path $root "lab02\lab02.exe"
$lab01ExeShort = Join-Path $rootShort "lab01\lab01.exe"
$lab02ExeShort = Join-Path $rootShort "lab02\lab02.exe"

Write-Host "Building..."
& gcc -std=c11 -O2 -Wall -Wextra -pedantic (Join-Path $rootShort "lab01\main.c") -o $lab01ExeShort
& gcc -std=c11 -O2 -Wall -Wextra -pedantic (Join-Path $rootShort "lab02\main.c") -o $lab02ExeShort

function Normalize([string]$s) {
    $n = ($s -replace "\r\n", "`n")
    return $n.TrimEnd()
}

$failed = 0

function Run-Test($name, $exe, [string[]]$exeArgs, $inputPath, $expectedPath, $outPath) {
    Write-Host "Running $name..."
    $out = Get-Content $inputPath | & $exe @exeArgs 2>&1 | Out-String
    $out = Normalize $out
    $exp = Normalize (Get-Content -Raw $expectedPath)

    if ($out -ne $exp) {
        $script:failed++
        Write-Host "FAIL: $name"
        $out | Set-Content -Encoding ASCII -Path $outPath
    } else {
        Write-Host "PASS: $name"
    }
}

$outDir = Join-Path $root "tests\out"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

for ($i = 1; $i -le 5; $i++) {
    $input = Join-Path $root "tests\lab01\input$i.txt"
    $expected = Join-Path $root "tests\lab01\expected$i.txt"
    $out = Join-Path $outDir "lab01_out$i.txt"
    Run-Test "lab01 test $i" $lab01ExeShort @("--quiet") $input $expected $out
}

for ($i = 1; $i -le 5; $i++) {
    $input = Join-Path $root "tests\lab02\input$i.txt"
    $expected = Join-Path $root "tests\lab02\expected$i.txt"
    $out = Join-Path $outDir "lab02_out$i.txt"
    Run-Test "lab02 test $i" $lab02ExeShort @("--quiet", "--seed", "1") $input $expected $out
}

if ($failed -gt 0) {
    Write-Host "Tests failed: $failed"
    exit 1
}

Write-Host "All tests passed."
