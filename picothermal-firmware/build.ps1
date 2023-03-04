#Todo:
# Add header
# Make it possible to build the project in multiple ways

$build_directory = "build"

function PrintHelp {
    Write-Output "
    'clean' - Clean the build environment `n
    'build Debug/Release' - Build the project in debug mode
    "
}

switch ($args[0])
{
    
    "clean" { 
        If (!(test-path -PathType container $build_directory)) { 
            Write-Output "Nothing to clean" 
        } Else { 
            Remove-Item -LiteralPath $build_directory -Force -Recurse } 
        }
    "build" {
        $build_types = @("Release", "Debug")
        $build_type = If ($args[1]) {$args[1]} Else {"Release"}

        If (!($build_type -in $build_types)) { 
            Write-Output "Unknown command! $build_type `n "
            PrintHelp 
            Exit
        }

        Write-Output "Build type: '$build_type'"

        If (!(test-path -PathType container $build_directory)) { 
            cmake -S . -B $build_directory -G"Ninja" -DCMAKE_BUILD_TYPE="$build_type" 
        }
        cmake --build $build_directory
    }
    default { Write-Output "Unknown command! `n "; PrintHelp}
}