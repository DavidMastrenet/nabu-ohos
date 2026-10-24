$ErrorActionPreference = 'Stop'
& "$PSScriptRoot\tools\python\python.exe" "$PSScriptRoot\scripts\install.py" deploy @args
exit $LASTEXITCODE
