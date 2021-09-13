package com.printer.phoneapp.Sockets;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import com.printer.phoneapp.Devices.ConnectDevice;

import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by hmwan on 2021/9/7.
 */

abstract public class SocketThread {
    protected ExecutorService mSocketThread = null;
    protected ConnectDevice mDevice = null;

    public interface OnSocketDataListener {
        public void onSent(ConnectDevice dev, byte[] sent);
        public void onReceived(ConnectDevice dev, byte[] recv, int length);
        public void onFailed(ConnectDevice dev, String errMsg);
    }

    public interface OnSocketStringListener {
        public void onSent(ConnectDevice dev, String sent);
        public void onReceived(ConnectDevice dev, String recv);
        public void onFailed(ConnectDevice dev, String errMsg);
    }

    public SocketThread(ConnectDevice dev) {
        mDevice = dev;
        mSocketThread = Executors.newCachedThreadPool();
    }

    abstract public void sendString(final String msg, final OnSocketStringListener l);
}
