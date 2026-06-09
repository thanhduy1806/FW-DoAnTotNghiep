#include "bsp_switch_to_adc.h"
#include "bsp_core.h"

#include "M3_Driver/devices/MAX14662/MAX14662.h"

#define SWITCH_ANA_1                                MAX14662_I2C_ADDRESS_SLAVE_1
#define SWITCH_ANA_2                                MAX14662_I2C_ADDRESS_SLAVE_2

extern i2c_io_t i2c1;

max14662_dev_t max14662_dev = {
    .i2c = &i2c1,
    .address_slave = MAX14662_I2C_ADDRESS_SLAVE_1,
};

void bsp_switch_to_adc_init(void)
{
    ((pio_registers_t*)PIO_PORT_C)->PIO_PER |= (1u << 17);
    ((pio_registers_t*)PIO_PORT_C)->PIO_PUER |= (1u << 17);
    ((pio_registers_t*)PIO_PORT_C)->PIO_OER |= (1u << 17);
    ((pio_registers_t*)PIO_PORT_C)->PIO_SODR |= (1u << 17);
    
    max14662_init(&max14662_dev);
}

uint8_t bsp_switch_to_adc_on(uint8_t switch_number)
{
    bsp_switch_to_adc_off_all(); // Tắt tất cả switch trước khi bật switch mới
    if(switch_number > 0 && switch_number < 9)
    {
        max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_1; // Chọn slave 1 cho switch 1-8
    }
    else if(switch_number > 8 && switch_number < 17)
    {
        max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_2; // Chọn slave 2 cho switch 9-16
        switch_number -= 8; // Điều chỉnh số switch để phù hợp với slave 2
    }
    else
    {
        return MAX14662_ERR; // Số switch không hợp lệ
    }

    uint8_t data = 0;
    // if(max14662_read(&i2c1, &data) != MAX14662_OK) return MAX14662_ERR; // Đọc dữ liệu hiện tại thất bại
    
    data = (1 << (switch_number - 1)); // Bật switch tương ứng
    
    if(max14662_write(&max14662_dev, &data) != MAX14662_OK) return MAX14662_ERR; // Ghi dữ liệu mới thất bại
    
    return data; // Trả về trạng thái mới của tất cả switch
}

uint8_t bsp_switch_to_adc_off(uint8_t switch_number)
{
    uint8_t data = 0;
    // if(max14662_read(&i2c1, &data) != MAX14662_OK) return MAX14662_ERR; // Đọc dữ liệu hiện tại thất bại
    if(switch_number > 0 && switch_number < 9)
    {
        max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_1; // Chọn slave 1 cho switch 1-8
    }
    else if(switch_number > 8 && switch_number < 17)
    {
        max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_2; // Chọn slave 2 cho switch 9-16
        switch_number -= 8; // Điều chỉnh số switch để phù hợp với slave 2
    }
    else
    {
        return MAX14662_ERR; // Số switch không hợp lệ
    }
    
    data &= ~(1 << (switch_number - 1)); // Tắt switch tương ứng
    
    if(max14662_write(&max14662_dev, &data) != MAX14662_OK) return MAX14662_ERR; // Ghi dữ liệu mới thất bại
    
    return data; // Trả về trạng thái mới của tất cả switch
}

uint8_t bsp_switch_to_adc_off_all(void)
{
    uint8_t data = 0;
    max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_1; // Chọn slave 1 cho switch 1-8
    if(max14662_write(&max14662_dev, &data) != MAX14662_OK) return MAX14662_ERR; // Ghi dữ liệu mới thất bại
    max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_2; // Chọn slave 2 cho switch 9-16
    if(max14662_write(&max14662_dev, &data) != MAX14662_OK) return MAX14662_ERR; // Ghi dữ liệu mới thất bại
    return MAX14662_OK;
}

uint8_t bsp_switch_to_adc_status(uint8_t switch_number)
{
    uint8_t data = 0;

    if(switch_number > 0 && switch_number < 9)
    {
        max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_1; // Chọn slave 1 cho switch 1-8
    }
    else if(switch_number > 8 && switch_number < 17)
    {
        max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_2; // Chọn slave 2 cho switch 9-16
        switch_number -= 8; // Điều chỉnh số switch để phù hợp với slave 2
    }
    else
    {
        return MAX14662_ERR; // Số switch không hợp lệ
    }
    
    if(max14662_read(&max14662_dev, &data) != MAX14662_OK) return MAX14662_ERR; // Đọc dữ liệu thất bại
    
    return (data >> (switch_number - 1)) & 0x01; // Trả về trạng thái của switch cụ thể
}

uint16_t bsp_all_switch_to_adc_status(void)
{
    uint16_t data = 0;
    uint8_t temp_data = 0;

    max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_1; // Đọc trạng thái từ slave 1 (switch 1-8)
    if(max14662_read(&max14662_dev, &temp_data) != MAX14662_OK) return MAX14662_ERR; // Đọc dữ liệu thất bại
    data = (uint16_t)temp_data;

    max14662_dev.address_slave = MAX14662_I2C_ADDRESS_SLAVE_2; // Đọc trạng thái từ slave 2 (switch 9-16)
    if(max14662_read(&max14662_dev, &temp_data) != MAX14662_OK) return MAX14662_ERR; // Đọc dữ liệu thất bại
    data |= ((uint16_t)temp_data << 8);

    return data; // Trả về trạng thái của tất cả switch
}

uint16_t bsp_get_adc_value_monitor(uint8_t switch_number)
{
    bsp_switch_to_adc_on(switch_number); // Bật switch tương ứng trước khi đọc ADC
    return AFEC0_ReadChannelFromSequence(ADC_POWER_CHANNEL); // Đọc giá trị ADC của kênh Power 
}

float bsp_get_voltage_value(uint8_t switch_number)
{
    bsp_switch_to_adc_on(switch_number); // Bật switch tương ứng trước khi đọc ADC
    uint16_t adc_value = AFEC0_ReadChannelFromSequence(ADC_POWER_CHANNEL);
    float voltage = (float)(adc_value - CALIBORATION_VALUE) * 3.3000f / 4095.0000f; // Chuyển đổi giá trị ADC sang điện áp
    return voltage;
}

float bsp_get_current_value_power(uint8_t switch_number)
{
    bsp_switch_to_adc_on(switch_number); // Bật switch tương ứng trước khi đọc ADC
    float voltage = (float)(AFEC0_ReadChannelFromSequence(ADC_POWER_CHANNEL) - CALIBORATION_VALUE)* 3.3000f / 4095.0000f; 
    float current = (voltage * 1000000) / (1600.0000f * 182); //  trở kháng là 1600 Ohm
    return current;
}

