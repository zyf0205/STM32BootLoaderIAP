import tkinter as tk  # 导入tkinter库，这是Python自带的标准GUI（图形界面）库
from tkinter import ttk, filedialog, messagebox  # 导入tkinter的扩展模块：ttk(更好看的组件), filedialog(文件选择框), messagebox(弹窗提示)
import serial  # 导入pyserial库，用于通过USB串口和硬件（如STM32）进行通信
import serial.tools.list_ports  # 导入串口工具，专门用来扫描电脑上当前插了哪些串口设备
import struct  # 导入struct库，用于将Python的数字变量转换成二进制字节流（硬件能听懂的格式）
import threading  # 导入多线程库，用于在后台执行耗时任务，防止界面卡死
import time  # 导入时间库，用于延时或计算超时

# ==================== 通信协议定义 ====================
# 这里定义的是上位机（电脑）和下位机（STM32）约定好的“暗号”
# 只有双方都遵守这个协议，才能正确传输文件

# 数据包头：告诉单片机，一个新的数据包开始了
# b"\x5a\xa5" 是字节串写法，对应16进制的 0x5A 和 0xA5
HEAD = b"\x5a\xa5"

# 命令ID：区分不同的操作类型
CMD_CONNECT = 0x01  # 连接命令：相当于跟单片机说 "你好，我要开始发文件了"
CMD_DATA    = 0x02  # 数据命令：相当于 "这是固件文件的一段数据"
CMD_FINISH  = 0x03  # 结束命令：相当于 "文件发完了，你可以重启运行了"

# 应答标志：单片机给电脑的反馈
ACK  = 0x06  # 肯定应答 (Acknowledge)：表示 "收到了，没问题"
NACK = 0x15  # 否定应答 (Negative Acknowledge)：表示 "出错了，请重发"

class BootloaderTool:
    def __init__(self, root):
        """
        初始化函数：程序启动时会自动执行这里
        root: 主窗口对象
        """
        self.root = root
        self.root.title("STM32 固件升级工具 (Bootloader)")  # 设置窗口标题
        self.root.geometry("500x380")  # 设置窗口大小：宽500像素，高380像素

        # 定义类成员变量
        self.ser = None          # 用于存放串口对象（连接后才会赋值）
        self.is_flashing = False # 状态标记：True表示正在升级中，防止用户重复点击

        # ==================== 界面布局 (UI) ====================
        
        # 1. 顶部区域：用于选择串口号和波特率
        frame_top = tk.Frame(root)  # 创建一个容器框架，方便把一排控件放在一起
        frame_top.pack(pady=10)     # 将框架放到窗口上，垂直方向留出10像素间距

        # 串口选择下拉框
        tk.Label(frame_top, text="端口:").pack(side=tk.LEFT, padx=5) # 标签
        self.combo_ports = ttk.Combobox(frame_top, width=15)         # 下拉框
        self.combo_ports.pack(side=tk.LEFT, padx=5)

        # 刷新按钮
        self.btn_refresh = tk.Button(frame_top, text="刷新列表", command=self.refresh_ports)
        self.btn_refresh.pack(side=tk.LEFT, padx=5)

        # 波特率输入框
        tk.Label(frame_top, text="波特率:").pack(side=tk.LEFT, padx=5)
        self.entry_baud = tk.Entry(frame_top, width=8)
        self.entry_baud.insert(0, "115200")  # 默认填入115200
        self.entry_baud.pack(side=tk.LEFT, padx=5)

        # 2. 文件选择区域
        frame_file = tk.Frame(root)
        frame_file.pack(pady=10)

        # 显示文件路径的输入框
        self.entry_file = tk.Entry(frame_file, width=40)
        self.entry_file.pack(side=tk.LEFT, padx=5)

        # 选择文件按钮
        # 点击按钮时，会触发 self.select_file 函数
        tk.Button(frame_file, text="浏览文件(.bin)", command=self.select_file).pack(side=tk.LEFT)

        # 3. 进度条
        # mode="determinate" 表示进度条是可以计算进度的（从0%到100%）
        self.progress = ttk.Progressbar(root, orient="horizontal", length=450, mode="determinate")
        self.progress.pack(pady=10)

        # 4. 日志显示区
        # Text组件用于显示多行文本，比如升级过程中的提示信息
        self.text_log = tk.Text(root, height=10, width=65)
        self.text_log.pack(pady=5)

        # 5. 开始按钮
        self.btn_start = tk.Button(root, text="开始升级", command=self.start_flash_thread, 
                                   bg="#87CEEB", font=("微软雅黑", 12, "bold"))
        self.btn_start.pack(pady=10)

        # 界面画好后，先自动刷新一次串口列表，方便用户直接选择
        self.refresh_ports()

    def log(self, msg):
        """
        辅助函数：用于在界面的日志框里打印信息
        msg: 要显示的字符串
        """
        current_time = time.strftime("%H:%M:%S", time.localtime()) # 获取当前时间
        log_msg = f"[{current_time}] {msg}\n"     # 拼凑成 "[12:00:00] 消息内容" 的格式
        self.text_log.insert(tk.END, log_msg)     # 插入到文本框的末尾
        self.text_log.see(tk.END)                 # 自动滚动到最后一行，保证看到最新消息

    def refresh_ports(self):
        """
        扫描电脑上所有的COM口，并填入下拉框
        """
        # list_ports.comports() 会返回一个列表，包含所有可用串口设备
        ports = serial.tools.list_ports.comports()
        # 我们只需要设备的名称（比如 "COM3" 或 "/dev/ttyUSB0"）
        port_list = [p.device for p in ports]
        self.combo_ports["values"] = port_list
        
        # 如果找到了串口，默认选中第一个，方便用户
        if port_list:
            self.combo_ports.current(0)

    def select_file(self):
        """
        弹出一个系统文件选择窗口，让用户选择固件
        """
        # askopenfilename 会暂停程序等待用户选择，返回选择的文件完整路径
        filename = filedialog.askopenfilename(filetypes=[("Binary Files", "*.bin")])
        if filename:
            self.entry_file.delete(0, tk.END)  # 清空输入框
            self.entry_file.insert(0, filename) # 填入新路径

    def calc_checksum(self, data):
        """
        计算校验和（CheckSum）
        作用：防止数据在传输过程中出错。
        原理：把所有字节加起来，只取最后8位（0~255）。
        """
        total = sum(data) # 将数据中所有字节求和
        return total & 0xFF  # 按位与运算，只保留最低的8位（即取模256）

    def send_packet(self, cmd, payload=b""):
        """
        核心函数：打包并发送数据
        协议格式：[头(2B)] [命令(1B)] [长度(2B)] [数据(N B)] [校验和(1B)]
        
        cmd: 命令类型 (如 CMD_DATA)
        payload: 要发送的具体数据（字节类型），默认为空
        """
        # 1. 获取数据长度
        length = len(payload)

        # 2. 将长度整数转换为2个字节的二进制
        # '<H' 的含义：
        # < : 小端模式 (Little Endian)，低位字节在前，STM32通常用这种
        # H : Unsigned Short，无符号短整型（占2个字节）
        len_bytes = struct.pack("<H", length)

        # 3. 拼接除包头和校验位之外的部分：命令 + 长度 + 数据
        # bytes([cmd]) 是把整数cmd转换成一个字节
        packet_content = bytes([cmd]) + len_bytes + payload

        # 4. 计算这部分的校验和
        checksum = self.calc_checksum(packet_content)

        # 5. 加上包头和校验位，组成最终的完整数据包
        final_packet = HEAD + packet_content + bytes([checksum])

        # 6. 通过串口发送出去
        self.ser.write(final_packet)

    def wait_ack(self, timeout=2):
        """
        等待单片机回复 ACK
        返回: True(成功) 或 False(失败/超时)
        """
        # 设定一个截止时间：当前时间 + 超时秒数
        end_time = time.time() + timeout

        while time.time() < end_time:
            # in_waiting 表示串口缓冲区里有多少字节没读
            if self.ser.in_waiting > 0:
                resp = self.ser.read(1) # 读取1个字节
                if resp == bytes([ACK]):
                    return True  # 收到ACK，成功
                elif resp == bytes([NACK]):
                    self.log("错误：单片机拒绝接收 (NACK)")
                    return False
        
        # 如果循环结束还没返回，说明超时了
        self.log("错误：等待响应超时")
        return False

    def start_flash_thread(self):
        """
        点击"开始升级"按钮后触发
        作用：创建一个新线程去干活，而不是在主线程里干活。
        为什么？因为如果在主线程里做耗时操作（如发文件），界面会卡死不动。
        """
        if not self.is_flashing: # 防止重复点击
            # target=self.flash_process 指定了新线程要执行的函数
            t = threading.Thread(target=self.flash_process)
            t.daemon = True  # 守护线程：如果主程序关闭，这个线程也会自动强制结束
            t.start()        # 启动线程

    def flash_process(self):
        """
        这是真正在后台执行升级逻辑的函数
        """
        # 1. 获取界面上的配置信息
        port = self.combo_ports.get()
        baud = self.entry_baud.get()
        file_path = self.entry_file.get()

        # 2. 简单的参数检查
        if not port or not file_path:
            messagebox.showwarning("提示", "请先选择串口和固件文件！")
            return

        try:
            # 3. 尝试打开串口
            self.log(f"正在打开串口 {port} ...")
            # timeout=0.1 表示读串口时如果0.1秒没数据就跳过，防止死等
            self.ser = serial.Serial(port, int(baud), timeout=0.1)
            
            self.is_flashing = True
            self.btn_start.config(state=tk.DISABLED) # 禁用按钮，防止用户乱点

            # 4. 读取电脑上的固件文件
            with open(file_path, "rb") as f: # "rb"表示以二进制只读模式打开
                firmware_data = f.read()
            
            total_len = len(firmware_data)
            self.log(f"文件读取成功，大小: {total_len} 字节")

            # 5. 发送连接命令（握手）
            self.log("正在尝试连接 Bootloader...")
            connected = False
            for i in range(5): # 尝试5次
                self.send_packet(CMD_CONNECT)
                if self.wait_ack(): # 如果收到ACK
                    connected = True
                    break
                time.sleep(0.1) # 稍微等一下再重试

            if not connected:
                self.log("连接失败：单片机没有响应。请检查连线或复位单片机。")
                return # 退出函数

            self.log("连接成功！开始发送数据...")

            # 6. 分包发送文件数据
            chunk_size = 1024 # 每次发送1024字节 (1KB)
            # 计算总共要发多少包：(总大小 + 包大小 - 1) // 包大小 是向上的整除法
            num_packets = (total_len + chunk_size - 1) // chunk_size

            for i in range(num_packets):
                # 计算当前包在文件中的起始和结束位置
                start = i * chunk_size
                end = min(start + chunk_size, total_len) # 防止最后一次超出文件长度
                
                chunk_data = firmware_data[start:end] # 切片操作，获取这段数据

                # 发送重试机制
                retry_count = 3
                while retry_count > 0:
                    self.send_packet(CMD_DATA, chunk_data) # 发送数据包
                    if self.wait_ack(): # 等待确认
                        break # 发送成功，跳出重试循环
                    
                    retry_count -= 1
                    self.log(f"第 {i+1} 包发送失败，正在重试 ({3-retry_count}/3)...")
                
                if retry_count == 0:
                    raise Exception("发送数据失败，多次重试无效")

                # 更新界面上的进度条
                progress_val = ((i + 1) / num_packets) * 100
                self.progress["value"] = progress_val
                # 提示：因为我们在子线程，理论上不该直接操作UI，但在Tkinter简单应用中通常没问题
                # 如果是严谨的开发，需要用事件队列通知主线程更新UI
                
            # 7. 发送结束命令
            self.log("数据发送完毕，正在发送结束指令...")
            self.send_packet(CMD_FINISH)
            
            if self.wait_ack():
                self.log("升级成功！固件已更新。")
                messagebox.showinfo("成功", "固件升级完成！")
            else:
                self.log("注意：未收到结束确认（可能单片机已直接跳转）")

        except Exception as e:
            # 捕获所有可能发生的错误（如串口被拔出、文件不存在等）
            self.log(f"发生异常: {e}")
            messagebox.showerror("错误", f"升级过程中出错：\n{e}")

        finally:
            # 8. 无论成功还是失败，最后都要执行的代码
            if self.ser and self.ser.is_open:
                self.ser.close() # 关闭串口，释放资源
            
            self.is_flashing = False
            self.btn_start.config(state=tk.NORMAL) # 恢复按钮可用
            self.log("串口已关闭。")

# ==================== 程序主入口 ====================
if __name__ == "__main__":
    # 创建主窗口实例
    root = tk.Tk()
    # 将主窗口传给我们的工具类，初始化界面
    app = BootloaderTool(root)
    # 进入主事件循环：程序开始运行，等待用户点击或键盘输入
    root.mainloop()