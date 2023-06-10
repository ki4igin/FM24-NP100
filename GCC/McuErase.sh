utility="C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"

if "$utility" -ME >&-
then 
echo -e "\033[32;1;40m""Flash memory erased""\033[0m"
else 
"$utility" -ME
fi