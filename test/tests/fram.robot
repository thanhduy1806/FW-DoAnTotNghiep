*** Settings ***
Documentation    FRAM tests for SAMV71 UART CLI.
Resource         ../resources/samv71_keywords.robot
Suite Setup      Connect To SAMV71
Suite Teardown   Disconnect From SAMV71

*** Test Cases ***
FRAM WR
    FRAM Should Return    PASS