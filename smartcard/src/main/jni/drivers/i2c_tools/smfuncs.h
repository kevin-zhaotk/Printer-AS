/*********************************************************************************
    smfuncs.h

    @功能
        i2c总线支持功能检测程序
    @创建
        2020.1.17
    @作者
        H.M. Wang
    @版权
        Copyrights 2019 - 2029, Allrights reserved.
    @地点
        北京
    @变更履历
        2020.1.17 依据i2c_tools的功能改写
*********************************************************************************/

#ifndef LIB_I2C_SMFUNCS_H
#define LIB_I2C_SMFUNCS_H

extern int check_read_byte(int file);
extern int check_write_byte(int file);
extern int check_read_byte_data(int file);
extern int check_write_byte_data(int file);
extern int check_read_word_data(int file);
extern int check_write_word_data(int file);
extern int check_read_block_data(int file);
extern int check_write_block_data(int file);
extern int check_read_i2c_block_data(int file);
extern int check_write_i2c_block_data(int file);
extern int check_process_call(int file);
extern int check_block_process_call(int file);
extern int check_pec(int file);

#endif /* LIB_I2C_SMFUNCS_H */
