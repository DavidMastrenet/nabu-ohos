$ErrorActionPreference = 'Stop'
& "$PSScriptRoot\tools\python\python.exe" "$PSScriptRoot\scripts\install.py" plan @args
exit $LASTEXITCODE
