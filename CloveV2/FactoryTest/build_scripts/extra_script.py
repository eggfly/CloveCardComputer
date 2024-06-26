import logging
import gen_esp32part

print(dir(gen_esp32part))

Import("env") # type: ignore

print("---- Processing build scripts by eggfly ----")
# print(Import)
# print(dir(env)) # type: ignore

storage_bin_path = "AlibabaPuHuiTi-3-65-Medium.ttf"  # 你的二进制文件路径
# partition_bin_path = env.subst("$BUILD_DIR/${PROGNAME}.bin")
# print("partition_bin_path:", partition_bin_path)
# python gen_esp32part.py ./.pio/build/CloveCardComputer/partitions.bin
def after_build(source, target, env):
    firmware_path = env.subst("$BUILD_DIR/${PROGNAME}.bin")
    print("firmware_path:", firmware_path)
    env.Execute("$UPLOADER $UPLOADERFLAGS      0x210000 " + storage_bin_path)

# env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", after_build)

def before_upload(source, target, env):
    print("before upload called", dir(env))
    # 获取当前的上传命令行参数
    uploader_flags = env.get("UPLOADERFLAGS", [])
    print(uploader_flags)
    #if "tft.bin" not in uploader_flags:
    #    uploader_flags.append("0x88")
    #    uploader_flags.append(storage_bin_path)
    
    env.Replace(UPLOADERFLAGS=uploader_flags)

# 将 before_upload 函数挂钩到上传前事件
env.AddPreAction("upload", before_upload)
