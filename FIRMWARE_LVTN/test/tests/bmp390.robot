*** Settings ***
Documentation    BMP390.
Resource         ../resources/samv71_keywords.robot
Suite Setup      Connect To SAMV71
Suite Teardown   Disconnect From SAMV71

*** Test Cases ***
Internal sensor returns data
    ${line}=    BMP390 Read Internal
    Log    ${line}

External sensor auto enable and read
    BMP390 Read External    True

External disable and read
    BMP390 Disable Switch
    ${resp}=    Execute Command And Expect    bmp390_ext_read    BMP390 switch is disble
    Should Contain    ${resp}    BMP390 switch is disble

External enable then read
    BMP390 Enable Switch
    ${line}=    BMP390 Read External
    Log    ${line}