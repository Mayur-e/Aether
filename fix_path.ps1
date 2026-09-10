# fix_path.ps1 — Remove old MinGW.org GCC 6.3.0 from System PATH
# Run as Administrator: Right-click > "Run with PowerShell" (choose Yes at UAC prompt)

$machinePath = [System.Environment]::GetEnvironmentVariable('PATH', 'Machine')
$entries = $machinePath -split ';' | Where-Object { $_ -ne '' -and $_ -ne 'C:\MinGW\bin' }
$newPath = $entries -join ';'

[System.Environment]::SetEnvironmentVariable('PATH', $newPath, 'Machine')

Write-Host ""
Write-Host "=== PATH FIX APPLIED ===" -ForegroundColor Green
Write-Host "Removed 'C:\MinGW\bin' from System PATH." -ForegroundColor Green
Write-Host ""
Write-Host "Verification -- MinGW entries remaining in System PATH:" -ForegroundColor Cyan
[System.Environment]::GetEnvironmentVariable('PATH', 'Machine') -split ';' | Where-Object { $_ -match 'mingw|MinGW' }
Write-Host ""
Write-Host "Done. Close and reopen your terminal for changes to take effect." -ForegroundColor Yellow
Read-Host "Press Enter to exit"
