*** Settings ***
Documentation     Test Power + ADC + Switch CLI
Resource          ../resources/samv71_keywords.robot
Suite Setup       Connect To SAMV71
Suite Teardown    Disconnect From SAMV71

*** Variables ***
@{SW_LIST}        5     6    7
@{TEC_LIST}       9    10   11   12   

*** Test Cases ***

# =========================
# 1. Read current
# =========================
Read Current All Channels
    FOR    ${sw}    IN    @{SW_LIST}
        ${resp}=    SW_TO_ADC Get Current Power   ${sw}
        Log    ${resp}
    END

# =========================
# 2. Read voltage TEC
# =========================
Read TEC Voltage
    FOR    ${ch}    IN    @{TEC_LIST}
        ${resp}=    SW_TO_ADC TEC Read Voltage    ${ch}
        Log    ${resp}
    END

# =========================
# 3. Switch ON/OFF single
# =========================
Switch ON OFF Channel 
    SW_TO_ADC ON    10
    ${status}=    SW_TO_ADC Get Status
    Should Be Equal    ${status}    0X0200

    SW_TO_ADC OFF   10
    ${status}=    SW_TO_ADC Get Status
    Should Be Equal    ${status}    0X0000

# =========================
# 4. Switch multiple (bitmask test)
# =========================
Switch Bitmask Test
    SW_TO_ADC ON    2
    ${status}=    SW_TO_ADC Get Status
    Should Be Equal    ${status}    0X0002

    SW_TO_ADC ON    4
    ${status}=    SW_TO_ADC Get Status
    Should Be Equal    ${status}    0X0008

    SW_TO_ADC ON    9
    ${status}=    SW_TO_ADC Get Status
    Should Be Equal    ${status}    0X0100

    SW_TO_ADC ON    10
    ${status}=    SW_TO_ADC Get Status
    Should Be Equal    ${status}    0X0200



