echo "[DEBUG] Appending filesystem to kernel"
# Append filesystem image to the kernel
dd bs=16MB if=build/programs/os.img >>build/kernel8.img
