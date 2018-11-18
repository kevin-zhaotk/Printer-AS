package com.industry.printer.Socket_Server;

import java.util.ArrayList;
import java.util.Deque;
import java.util.LinkedList;
import java.util.List;

interface FIFO<T> extends List<T>, Deque<T>, Cloneable, java.io.Serializable {  
	  
    /** 
     * 向最后添加一个新的，如果长度超过允许的最大值，则弹出一个 * 
     */  
    T addLastSafe(T addLast);  
  
    /** 
     * 弹出head，如果Size = 0返回null。而不同于pop抛出异常 
     * @return  
     */  
    T pollSafe();  
  
    /** 
     * 获得最大保存 
     * 
     * @return 
     */  
    int getMaxSize();  
  
    /** 
     * 设置最大存储范围 
     * 
     * @return 返回的是，因为改变了队列大小，导致弹出的head 
     */  
    List<T> setMaxSize(int maxSize);  
  
}  
  
class FIFOImpl<T> extends LinkedList<T> implements FIFO<T> {  
  
    private int maxSize = Integer.MAX_VALUE;  
    private final Object synObj = new Object();  
  
    public FIFOImpl() {  
        super();  
    }  
  
    public FIFOImpl(int maxSize) {  
        super();  
        this.maxSize = maxSize;  
    }  
  
    @Override  
    public T addLastSafe(T addLast) {  
        synchronized (synObj) {  
            T head = null;  
            while (size() >= maxSize) {  
                head = poll();  
            }  
            addLast(addLast);  
            return head;  
        }  
    }  
  
    @Override  
    public T pollSafe() {  
        synchronized (synObj) {  
            return poll();  
        }  
    }  
  
    @Override  
    public List<T> setMaxSize(int maxSize) {  
        List<T> list = null;  
        if (maxSize < this.maxSize) {  
            list = new ArrayList<T>();  
            synchronized (synObj) {  
                while (size() > maxSize) {  
                    list.add(poll());  
                }  
            }  
        }  
        this.maxSize = maxSize;  
        return list;  
    }  
  
    @Override  
    public int getMaxSize() {  
        return this.maxSize;  
    }  
}  
  
class A {  
  
    private String counter;  
  
    public A() {  
    }  
  
    public A(String counter) {  
        this.counter = counter;  
    }  
  
    public String getcounter() {  
        return counter;  
    }  
  
    public void setpath_name(String counter) {  
        this.counter = counter;  
    }  
  
    @Override  
    public String toString() {  
        return "A{" + "counter=" + counter + '}';  
    }  
} 