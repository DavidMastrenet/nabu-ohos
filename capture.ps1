$ErrorActionPreference = 'Stop'
& "$PSScriptRoot\tools\python\python.exe" "$PSScriptRoot\scripts\install.py" capture @args
exit $LASTEXITCODE
