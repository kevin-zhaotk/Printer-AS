package com.printer.phoneapp.Sockets;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import com.printer.phoneapp.Devices.ConnectDevice;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.UUID;

/**
 * Created by hmwan on 2021/9/7.
 */

public class BTSocketThread extends SocketThread {
    private final String PRINTER_BT_UUID = "12345678-0000-0000-0000-00805f9b34fb";

    public BTSocketThread(ConnectDevice dev) {
        super(dev);
    }

    @Override
    public void sendString(final String msg, final OnSocketStringListener l) {
        mSocketThread.execute(new Runnable() {
            @Override
            public void run() {
                BluetoothSocket socket = null;
                OutputStream os = null;
//                DataInputStream dis = null;
//                byte[] recv = new byte[8192];
                BufferedReader br = null;
                try {
                    BluetoothManager mBTManager = BluetoothManager.getInstance();
                    if(null == mBTManager) {
                        if(null != l) l.onFailed(mDevice, "Not initialized");
                        return;
                    }
                    if(!mBTManager.isSupported()) {
                        if(null != l) l.onFailed(mDevice, "Don't support Bluetooth");
                        return;
                    }
//                    if(!mBTManager.isEnabled()) {
//                        mBTManager.enableBluetooth();
//                        while(mBTManager.isEnabling()){Thread.sleep(100);}
//                    }

                    if(!mBTManager.isEnabled()) {
                        if(null != l) l.onFailed(mDevice, "Don't enabled");
                        return;
                    }

                    BluetoothDevice device = mBTManager.getAdapter().getRemoteDevice(mDevice.getAddress());
                    socket = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString(PRINTER_BT_UUID));
                    if(null != socket && !socket.isConnected()) {
                        socket.connect();
                    }

                    if(null != socket) {
                        br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                        br.readLine();

                        os = socket.getOutputStream();
                        os.write(msg.getBytes());
                        os.write(new byte[]{0x0A});
                        os.flush();
                        if (null != l) l.onSent(mDevice, msg);

//                        dis = new DataInputStream(socket.getInputStream());
//                        int length = dis.read(recv);
//                        if (null != l) l.onReceived(mDevice, recv, length);
                        String is = br.readLine();
                        if (null != l) l.onReceived(mDevice, is);
                    }
                } catch (Exception e) {
                    if(null != l) l.onFailed(mDevice, "Error: " + e.getMessage());
                } finally {
                    try {
                        if(null != os) {
                            os.close();
                        }
                        if(null != br) {
                            br.close();
                        }
                        if(null != socket) {
                            socket.close();
                        }
                    } catch (Exception e) {
                    }
                }
            }
        });
    }
}
