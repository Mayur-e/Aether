$env:PATH = "D:\msys64\ucrt64\bin;" + $env:PATH; Write-Host "Starting Game..."; .\build\AETHER.exe; Write-Host "Game exited with code $LASTEXITCODE"
