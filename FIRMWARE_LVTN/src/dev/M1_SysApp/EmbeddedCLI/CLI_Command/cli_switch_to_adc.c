#include "cli_switch_to_adc.h"
#include "M2_BSP/BSP_Switch_To_ADC/bsp_switch_to_adc.h"

void CMD_Switch_On_To_ADC(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    char buf[128];
    uint8_t switch_num = (uint8_t)strtoul(arg1, NULL, 0);;

    // snprintf(buf, sizeof(buf), "Usage: switch_on_adc <%d>", switch_num);
    // embeddedCliPrint(cli, buf);

    bsp_switch_to_adc_on(switch_num);
    snprintf(buf, sizeof(buf), "Switch to ADC mode ON with parameter: %d", switch_num);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");  
}

void CMD_Switch_Off_To_ADC(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    char buf[128];
    uint8_t switch_num = (uint8_t)strtoul(arg1, NULL, 0);
    
    // snprintf(buf, sizeof(buf), "Usage: switch_off_adc <%d>", switch_num);
    // embeddedCliPrint(cli, buf);

    bsp_switch_to_adc_off(switch_num);
    snprintf(buf, sizeof(buf), "Switch to ADC mode OFF with parameter: %d", switch_num);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");  
}

void CMD_Get_ADC_Value_Monitor(EmbeddedCli *cli, char *args, void *context)
{
    const char *arg1 = embeddedCliGetToken(args, 1);
    char buf[128];
    uint8_t switch_num = (uint8_t)strtoul(arg1, NULL, 0);
    // snprintf(buf, sizeof(buf), "Usage: get_switch <%d>", switch_num);
    // embeddedCliPrint(cli, buf);

    // Chỉ bật 1 switch trước khi đọc ADC 
    uint16_t adc_value = bsp_get_adc_value_monitor(switch_num);
    snprintf(buf, sizeof(buf), "ADC Value Monitor: %lu", adc_value);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Get_All_Switch_Status(EmbeddedCli *cli, char *args, void *context)
{
    uint16_t status = bsp_all_switch_to_adc_status();
    char buf[128];
    snprintf(buf, sizeof(buf), "All Switch Status: 0X%04X", status);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Get_Current_Value_Power(EmbeddedCli *cli, char *args, void *context)
{
    uint16_t tok_count = embeddedCliGetTokenCount(args);
    if (tok_count < 1) {
        embeddedCliPrint(cli, "Usage: get_current_power <switch_number>");
        embeddedCliPrint(cli, " 1  :   12V_IF");
        embeddedCliPrint(cli, " 2  :   12V_HEATER");
        embeddedCliPrint(cli, " 3  :   12V_LP");
        embeddedCliPrint(cli, " 4  :   5V_SOM");
        embeddedCliPrint(cli, " 5  :   5V_TEC");
        embeddedCliPrint(cli, " 6  :   5V_HD4");
        embeddedCliPrint(cli, " 7  :   3/5V_SOLENOID");
        return;
    }

    const char *arg1 = embeddedCliGetToken(args, 1);
    char buf[128];
    uint8_t switch_num = (uint8_t)strtoul(arg1, NULL, 0);
    if(switch_num < 1 || switch_num > 7) {
        embeddedCliPrint(cli, "Invalid switch number. Valid range: 1-7");
        return;
    }
    // snprintf(buf, sizeof(buf), "Usage: get_switch <%d>", switch_num);
    // embeddedCliPrint(cli, buf);

    float current_value = bsp_get_current_value_power(switch_num);
    snprintf(buf, sizeof(buf), "Current Value Power: %.4f", current_value);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}

void CMD_Get_Voltage_Value(EmbeddedCli *cli, char *args, void *context)
{
    uint16_t tok_count = embeddedCliGetTokenCount(args);
    if (tok_count < 1) {
        embeddedCliPrint(cli, " 9  :   TEC_ADC_1");
        embeddedCliPrint(cli, " 10 :   TEC_ADC_2");
        embeddedCliPrint(cli, " 11 :   TEC_ADC_3");
        embeddedCliPrint(cli, " 12 :   TEC_ADC_4");
        return;
    }

    const char *arg1 = embeddedCliGetToken(args, 1);
    char buf[128];
    uint8_t switch_num = (uint8_t)strtoul(arg1, NULL, 0);

    if(switch_num < 9 || switch_num > 12) {
        embeddedCliPrint(cli, "Invalid switch number. Valid range: 9-12");
        return;
    }
    // snprintf(buf, sizeof(buf), "Usage: get_switch <%d>", switch_num);
    // embeddedCliPrint(cli, buf);

    float voltage_value = bsp_get_voltage_value(switch_num);
    snprintf(buf, sizeof(buf), "Voltage Value: %.4f", voltage_value);
    embeddedCliPrint(cli, buf);
    embeddedCliPrint(cli, "");
}