echo -e "${BIYellow}[DEBUG] Building file system${Color_Off}"

mount_point="/media/os_img_mount"
sudo mkdir -p $mount_point

echo "[DEBUG] Mounting os.img"
loop_device=$(sudo losetup -f --show -o 32256 src/os.img)
sudo mount -t msdos $loop_device $mount_point

sudo rm -rf $mount_point/*

echo "[DEBUG] Copying files to os.img"
sudo find build -name "*.bin" -exec cp {} $mount_point/ \;

echo "[DEBUG] Contents of os.img:"
ls $mount_point

echo "[DEBUG] Unmounting os.img"
sudo umount $mount_point
sudo losetup -d $loop_device
