package com.industry.printer.hardware;

import android.os.Handler;

public interface IInkDevice {

    /**
     * 设备初始化
     * 初始化成功要调用callback发送@{RFIDManager.MSG_RFID_INIT_SUCCESS}消息
     * 初始化失败要调用callback发送@{RFIDManager.MSG_RFID_INIT_FAIL}消息
     * @param callback
     */
    void init(final Handler callback);


    /**
     * 检查设备的UUID，用来判断设备是否发生变化
     * @param heads 一共要校验的喷头数量
     * return RFIDManager.MSG_RFID_CHECK_FAIL 表示设备发生变化
     *        RFIDManager.MSG_RFID_CHECK_SUCCESS 表示设备未发生变化
     */
    boolean checkUID(int heads);

    /**
     * 获取指定喷头对应的锁值
     * @param head 喷头索引
     * @return
     */
    float getLocalInk(int head);

    /**
     * 获取指定喷头对应的锁值
     * @param head 喷头索引
     * @return
     */
    float getLocalInkPercentage(int head);

    /**
     * 判断设备是否有效
     * 如：RFID如果feature code与固定值不一致就认为设备不合法
     *    SmartCard - 需要王工来指定合法规则
     * @param dev  设备编号 （0...n)
     * @return
     */
    boolean isValid(int dev);

    /**
     * 获取设备Feature信息
     * @param device
     * @param index
     * @return
     */
    int getFeature(int device, int index);


    /**
     * 减锁值
     * @param dev
     */
    void downLocal(int dev);

    /**
     * 切换锁设备
     * @param i
     */
    void switchRfid(final int i);


    /**
     * 忽略设备时的默认锁值
     */
    void defaultInkForIgnoreRfid();
}
