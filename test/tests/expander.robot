*** Settings ***
Documentation    Smoke tests for SAMV71 UART CLI.
Resource         ../resources/samv71_keywords.robot
Suite Setup      Connect To SAMV71
Suite Teardown   Disconnect From SAMV71

*** Test Cases ***
Expander Write and Read Check
    Expander Write Read Value    0x30

Expander Set Solenoid Check
    Expander Turn On Return      ON

Expander Clear Solenoid Check
    Expander Turn Off Return     OFF