# 查找 /dev/cu.usbmodem 开头的设备
device=$(ls /dev/cu.usbmodem* 2> /dev/null | head -n 1)

# 检查是否找到了设备
if [ -z "$device" ]; then
    echo "未找到 /dev/cu.usbmodem 设备"
    exit 1
else
    echo "找到设备: $device"
fi

# 使用找到的设备进行后续操作
if ! command -v esptool &> /dev/null
then
    esptool_cmd="$HOME/Library/Arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool"
else
    esptool_cmd="esptool"
fi

$esptool_cmd -p $device write_flash 0xb10000 ./AlibabaPuHuiTi-3-65-Medium.ttf
$esptool_cmd -p $device write_flash 0x1410000 ./wenquanyi_weimihei.ttf
