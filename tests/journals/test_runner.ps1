$audacityExecutable=$args[0]
$journalFile=$args[2]

Write-Host "Audacity Executable: $audacityExecutable"
Write-Host "Journal File: $journalFile"

if (-not(Test-Path -Path $journalFile -PathType Leaf)) {
    Write-Host "Journal file does not exist"
    exit 1
}

$process = [Diagnostics.Process]::Start("$audacityExecutable", "--journal $journalFile")

$completedInTime = $process.WaitForExit(180000)

if (-not $completedInTime) {
    Write-Host "Timed out waiting for Audacity to finish"
    exit 1
}

Write-Host "Audacity finished in $(($process.ExitTime - $process.StartTime).TotalSeconds) seconds"
Write-Host "Exit code: $($process.ExitCode)"

if ($process.ExitCode -ne 0) {
   $logDir = "$env:APPDATA\audacity"
   Write-Host "Audacity exited with an error, gathering data from $logDir"

   if (Test-Path -Path "$logDir\audacity.cfg" ) {
      Get-Content -Path "$logDir\audacity.cfg" | Out-Default
   } else {
      Write-Host "No audacity.cfg file found"
   }

   if (Test-Path -Path "$logDir\lastlog.txt" ) {
      Get-Content -Path "$logDir\lastlog.txt" | Out-Default
   } else {
      Write-Host "No lastlog.txt file found"
   }

   exit 1
}
