from PIL import Image

# 读取 PNG 图像
img = Image.open('music_player_536_240.png')

# 将图像转换为 RGB 模式
img = img.convert('RGB')

# 获取图像的宽度和高度
width, height = img.size

# 创建一个空的 C 数组结构
c_array = "const uint16_t image_data[] = {\n"

# 遍历图像的每个像素，并将 RGB 值转换为 RGB565 格式
for y in range(height):
    for x in range(width):
        r, g, b = img.getpixel((x, y))
        rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        c_array += f"0x{rgb565:04X}, "
    c_array += "\n"

c_array += "};"

# 将 C 数组结构保存到文件
with open('output.c', 'w') as file:
    file.write(c_array)

print("C 数组结构已保存到 output.c 文件")
