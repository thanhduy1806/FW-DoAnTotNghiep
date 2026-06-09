*** Settings ***
Documentation    LSM6DS0X tests for SAMV71 UART CLI.
Resource         ../resources/samv71_keywords.robot
Suite Setup      Connect To SAMV71
Suite Teardown   Disconnect From SAMV71

*** Test Cases ***
LSM6DS0X Self Test
    LSM6DS0X Should Return    OK