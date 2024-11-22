echo "Requesting root access: "
clear

rm -rf build
mkdir build

source scripts/colors.sh

echo -e "${BCyan}[DEBUG] Building OS${Color_Off}"

source scripts/build_programs.sh

source scripts/build_file_system.sh

source scripts/build_kernel.sh

source scripts/dump_kernel.sh

echo -e "${BGreen}[DEBUG] OS Built${Color_Off}"
