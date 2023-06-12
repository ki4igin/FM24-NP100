fileName=${PWD##*/}
fileExt=".hex"
fileNameFull=$filePath$fileName$fileExt
echo "Download file: ${fileNameFull}"
utility="C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"

if "$utility" -P $fileNameFull 0x08000000
then
echo | "$utility" -Rst -Run >&-
echo -e "\033[0;32m""Appication Run""\033[0m"
fi