::Compress packaged SDK & the source code

::::Create a directory that will hold the compressed files
mkdir "%WORKSPACE%\\artifacts"

::::Compress the package project
"%ZIP_PATH%\\7z.exe" a -t7z "%WORKSPACE%\\artifacts\\AccelByteUe4SdkPackage-%GIT_COMMIT%-BUILD-%BUILD_NUMBER%.7z" "%WORKSPACE%\\temp\\project"

::::Compress the package plugin
"%ZIP_PATH%\\7z.exe" a -t7z "%WORKSPACE%\\artifacts\\AccelByteUe4SdkSource-%GIT_COMMIT%-BUILD-%BUILD_NUMBER%.7z" "%WORKSPACE%\\temp\\plugin"