package com.industry.printer.Rfid;

public interface IInkScheduler {

    /**
     * initialization
     */
    void init(int heads);

    /**
     * count of tasks tobe schedule by this scheduler
     * @return
     */
    int count();


    /**
     * do schedule
     */
    void schedule();

    /**
     * do something after printing finished
     */
    void doAfterPrint();
}
