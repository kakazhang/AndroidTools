#!/system/bin/sh
dstname=$1
size=$2
loopdev=$4
mount_point=$5

echo "Please input 1:dstname, 2:size, 3:src, 4:loopdev, 5:mount_point"

BIN=/system/bin/make_ext4fs

$BIN -l $size $dstname $src

/system/xbin/losetup $loopdev $dstname

/system/bin/mount -t ext4 $loopdev $mount_point
