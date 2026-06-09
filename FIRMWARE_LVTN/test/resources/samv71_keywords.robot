*** Settings ***
Library    ../libraries/samv71_serial.py
Library    BuiltIn

*** Variables ***
#${SERIAL_PORT}          /dev/ttyUSB0

${SERIAL_PORT}          COM11
${BAUDRATE}             115200
${TIMEOUT_S}            1.0
${NEWLINE}              \r\n
*** Keywords ***
Connect To SAMV71
    Open SAMV71 Connection
    ...    port=${SERIAL_PORT}
    ...    baudrate=${BAUDRATE}
    ...    timeout_s=${TIMEOUT_S}
    ...    newline=${NEWLINE}
    ...    open_retries=5
    ...    command_retries=2
    ...    retry_delay_s=0.5
    ...    log_dir=logs

Disconnect From SAMV71
    Close SAMV71 Connection

Board Should Boot Normally
    ${banner}=    Wait For Boot Banner    expected=SAMV71    timeout_s=10.0
    Log    ${banner}

Ping Should Reply Pong
    ${resp}=    Execute Command And Expect    ping    pong    timeout_s=3.0    retries=3
    Should Contain    ${resp}    pong

Firmware Version Should Be Reported
    ${resp}=    Execute Command And Expect    version    SAMV71    timeout_s=3.0    retries=2
    Should Contain    ${resp}    SAMV71

Self Test Should Pass
    Wait Until Keyword Succeeds    3x    2s    Execute Command And Expect    selftest    SELFTEST: PASS    10.0    1

Echo Should Return Argument
    [Arguments]    ${text}
    ${resp}=    Execute Command And Expect    echo ${text}    ${text}    timeout_s=3.0    retries=2
    Should Contain    ${resp}    ${text}

Expander Write Read Value
    [Arguments]    ${value}
    Send Command    expander_write ${value}
    Execute Command And Expect    expander_read ${value}    ${value}    timeout_s=3.0    retries=2

Expander Turn On Return
    [Arguments]    ${value}
    Send Command    sol_single_on 9
    Execute Command And Expect    sol_single_get 9    ${value}    timeout_s=3.0    retries=2

Expander Turn Off Return
    [Arguments]    ${value}
    Send Command    sol_single_off 9
    Execute Command And Expect    sol_single_get 9    ${value}    timeout_s=3.0    retries=2


BMP390 Read Internal
    ${line}=    Execute Command And Expect    bmp390_int_read    Pa
    Should Contain    ${line}    Temp
    Should Contain    ${line}    Pressure
    RETURN    ${line}


BMP390 Read External
    [Arguments]    ${auto_enable}=False
    # try read; if disabled and auto_enable requested, enable and read again
    ${status}    ${resp}=    Run Keyword And Ignore Error    Execute Command And Expect    bmp390_ext_read    Pa
    IF    '${status}' == 'FAIL' and '${auto_enable}' == 'True'
        BMP390 Enable Switch
        ${resp}=    Execute Command And Expect    bmp390_ext_read    Pa
    ELSE
        Should Be Equal    ${status}    PASS
    END
    Should Contain    ${resp}    Temp
    Should Contain    ${resp}    Pressure
    RETURN    ${resp}

BMP390 Disable Switch
    ${resp}=    Execute Command And Expect    bmp390_ext_dis    Disable switch i2c bmp390 connector
    RETURN    ${resp}

BMP390 Enable Switch
    Send Command    bmp390_ext_ena
    ${resp}=    Execute Command And Expect    bmp390_ext_ena    SW is enabled
    RETURN    ${resp}

NTC Should Return
    [Arguments]    ${value}
    Execute Command And Expect    ntc_check OK    ${value}    timeout_s=1.0    retries=2

Solenoid Check Status
    Send Command    power_buck_peri_on
    Send Command    power_solenoid_on
    Send Command    sol_dual_forward 1
    ${result}=    Ask User To Verify    Did solenoid open? (yes/no)
    Should Be Equal    ${result}    yes    msg=FAIL: Solenoid NOT WORKING

Ask User To Verify
    [Arguments]    ${message}
    Log    ${message}    console=True
    ${answer}=    Evaluate    input("\\n[USER INPUT] ${message}: ").strip().lower()
    RETURN    ${answer}

RTC Should Return
    [Arguments]    ${message}
    Execute Command And Expect    rtc_ping OK    ${message}    timeout_s=1.0    retries=2

RTC Self Test
    [Arguments]    ${message}
    Execute Command And Expect    rtc_test OK    ${message}    timeout_s=2.0    retries=2

TEC Should Return
    [Arguments]    ${message}
    Send Command    power_buck_peri_on
    Send Command    power_tec_on
    Execute Command And Expect    tec_init 1    ${message}    timeout_s=1.0    retries=2

PUMP Check Status
    Send Command    pump_enable
    Send Command    pump_set_freq 100
    Send Command    pump_set_volt 150
    ${result}=    Ask User To Verify    Did PUMP run? (yes/no)
    Should Be Equal    ${result}    yes    msg=FAIL: PUMP NOT WORKING
    
FRAM Should Return
    [Arguments]    ${message}
    Execute Command And Expect    fram_write_read    ${message}    timeout_s=1.0    retries=2

PSRAM Should Return
    [Arguments]    ${message}
    Execute Command And Expect    psram_id    ${message}    timeout_s=1.0    retries=2

FLOW_SEN Should Return
    [Arguments]    ${message}
    Execute Command And Expect    flow_read_all    ${message}    timeout_s=1.0    retries=2


SW_TO_ADC Get Current Power
    [Arguments]    ${sw}
    ${line}=    Execute Command And Expect    get_i_pwr ${sw}    Current Value Power
    Sleep    5ms
    Should Match Regexp    ${line}    Current Value Power: 
    RETURN    ${line}


SW_TO_ADC TEC Read Voltage
    [Arguments]    ${ch}
    ${line}=    Execute Command And Expect    get_v_tec ${ch}    Voltage
    Sleep    100ms
    Should Match Regexp    ${line}    Voltage Value: 
    RETURN    ${line}

SW_TO_ADC ON
    [Arguments]    ${ch}
    Execute Command And Expect    sw_on ${ch}    ON
    Sleep    5ms

SW_TO_ADC OFF
    [Arguments]    ${ch}
    Execute Command And Expect    sw_off ${ch}    OFF
    Sleep    5ms

SW_TO_ADC Get Status
    ${resp}=    Execute Command And Expect    sw_status    All Switch Status
    ${status}=    Evaluate    __import__('re').search(r'0X[0-9A-F]+', '''${resp}''').group()
    RETURN    ${status}

LSM6DS0X Should Return
    [Arguments]    ${message}
    Execute Command And Expect   lsm6d_check OK    ${message}    timeout_s=2.0    retries=2