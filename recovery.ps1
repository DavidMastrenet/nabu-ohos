$ErrorActionPreference = 'Stop'
& "$PSScriptRoot\tools\python\python.exe" "$PSScriptRoot\scripts\install.py" recovery @args
exit $LASTEXITCODE
