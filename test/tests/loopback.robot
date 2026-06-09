*** Settings ***
Documentation    Simple payload loopback validation.
Resource         ../resources/samv71_keywords.robot
Suite Setup      Connect To SAMV71
Suite Teardown   Disconnect From SAMV71

*** Test Cases ***
Echo Small Payload
    Echo Should Return Argument    hello_samv71

Echo Payload With Spaces
    Echo Should Return Argument    firmware cli ready
