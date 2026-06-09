#*** Settings ***
#Documentation    Smoke tests for SAMV71 UART CLI.
#Resource         ../resources/samv71_keywords.robot
#Suite Setup      Connect To SAMV71
#Suite Teardown   Disconnect From SAMV71

#*** Test Cases ***
# Boot Banner Is Visible
#     Board Should Boot Normally

# Ping Command Works
#     Ping Should Reply Pong

#Version Command Works
#   Firmware Version Should Be Reported

# Selftest Passes
#     Self Test Should Pass
