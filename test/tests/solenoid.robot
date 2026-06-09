*** Settings ***
Documentation    Solenoid tests for SAMV71 UART CLI.
Resource         ../resources/samv71_keywords.robot
Suite Setup      Connect To SAMV71
Suite Teardown   Disconnect From SAMV71

*** Test Cases ***
Solenoid Check
    Solenoid Check Status    