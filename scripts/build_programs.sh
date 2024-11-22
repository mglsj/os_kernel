echo -e "${BIYellow}[DEBUG] Building Programs${Color_Off}"

source scripts/build_lib.sh

rm -rf build/programs
mkdir build/programs

source scripts/programs/build_test.sh
source scripts/programs/build_init.sh
source scripts/programs/build_console.sh
source scripts/programs/build_ls.sh
