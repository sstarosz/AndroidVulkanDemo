# Get the path of cmake from the system's PATH
$cmakePath = (Get-Command cmake).Source

# Get the path of the CMakePresets.json file in the same directory as the script
$presetsPath = Join-Path -Path $PSScriptRoot -ChildPath "CMakePresets.json"
echo "Using presets file: $presetsPath"

# Load the presets from the CMakePresets.json file
$presets = Get-Content -Path $presetsPath | ConvertFrom-Json

# Configure and build all Android configurations
foreach ($config in $presets.buildPresets | Where-Object { $_.configurePreset -like "Android*" })
{
    echo "Configuring and building $($config.name)"
    echo "Using preset $($config.configurePreset)"
    & $cmakePath --preset=$($config.configurePreset)
    & $cmakePath --build --preset=$($config.name)
}