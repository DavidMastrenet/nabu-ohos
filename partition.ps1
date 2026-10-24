$ErrorActionPreference = 'Stop'
& "$PSScriptRoot\tools\python\python.exe" "$PSScriptRoot\scripts\install.py" partition @args
exit $LASTEXITCODE
