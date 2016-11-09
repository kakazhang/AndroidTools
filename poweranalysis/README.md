原理： 通过对Android 5.X的机器进行分析，获取对应的Suspend blocks,以及wake lock size等信息
然后分析系统是否有多余的WakeLock在系统屏幕关闭后还被hold,通过初步判断系统power系统是否正常。

方式：
    dumpsys power 数据到 /mnt/sdcard/power.txt
    分析power.txt 信息

