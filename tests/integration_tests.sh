#!/bin/bash

#run the webserver
../build/bin/webserver ../config/integration_test.conf &

#server info
SERVER_PID=$!
SERVER_IP=localhost
SERVER_PORT=800

#timeout variable
TIMEOUT=0.2

#Make sure server is fully running
sleep 5

#Test 1: testing http response through curl
curl -s -o -S "Host:" "User-Agent:" $SERVER_IP:$SERVER_PORT > temp_file_test1

diff temp_file_test1 integration_test1_result

RESULT=$?

TEST_1_RESULT=0

if [ $RESULT -eq 0 ];
then
   TEST_1_RESULT=1
   echo "Test 1 Success"
else
   echo "Test 1 Fail"
fi

#Test 2: testing a correct echo response through nc
echo -e "GET /echo/testing HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test2

diff temp_file_test2 integration_test2_result

RESULT=$?

TEST_2_RESULT=0

if [ $RESULT -eq 0 ];
then
    TEST_2_RESULT=1
    echo "Test 2 Success"
else
    echo "Test 2 Fail"
fi

#Test 3: testing an incorrect response through nc
echo -e "hello" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test3

diff temp_file_test3 integration_test3_result

RESULT=$?

TEST_3_RESULT=0
if [ $RESULT -eq 0 ];
then
    TEST_3_RESULT=1
    echo "Test 3 Success"
else
    echo "Test 3 Fail"
fi

#Test 4: testing retrieval of static file

echo -e "GET /static1/hello.txt HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test4

diff temp_file_test4 integration_test4_result

RESULT=$?

TEST_4_RESULT=0
if [ $RESULT -eq 0 ];
then
    TEST_4_RESULT=1
    echo "Test 4 Success"
else
    echo "Test 4 Fail"
fi

#Test 5: testing invalid static file

echo -e "GET /static1/helloy.txt HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test5

diff temp_file_test5 integration_test5_result

RESULT=$?

TEST_5_RESULT=0
if [ $RESULT -eq 0 ];
then
    TEST_5_RESULT=1
    echo "Test 5 Success"
else
    echo "Test 5 Fail"
fi

#Test 6: testing health handler

echo -e "GET /health/ HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test6

diff temp_file_test6 integration_test6_result

RESULT=$?

TEST_6_RESULT=0
if [ $RESULT -eq 0 ];
then
    TEST_6_RESULT=1
    echo "Test 6 Success"
else
    echo "Test 6 Fail"
fi

#Test 7: confirm multithreading, and ensure sleep handler works

echo -e "GET /sleep/abc HTTP/1.1\r\n\r\n" | timeout 6 nc $SERVER_IP $SERVER_PORT > temp_file_test7 &
echo -e "GET /echo/abc HTTP/1.1\r\n\r\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test7 &
sleep .5

#shows echo gives response first
diff temp_file_test7 integration_test7_echo_result

RESULT=$?

TEST_7_ECHO_RESULT=0
if [ $RESULT -eq 0 ];
then
    TEST_7_ECHO_RESULT=1
    echo "Test 7 Echo recieved first"
else
    echo "Test 7 Echo not recieved"
fi

sleep 6

#checks correct response of sleep handler
diff temp_file_test7 integration_test7_sleep_result

RESULT=$?

TEST_7_SLEEP_RESULT=0
if [ $RESULT -eq 0 ];
then
    TEST_7_SLEEP_RESULT=1
    echo "Test 7 sleep handler gives correct response"
else
    echo "Test 7 sleep handler gives incorrect response"
fi


if [ $TEST_7_SLEEP_RESULT -eq 1 ] && [ $TEST_7_ECHO_RESULT -eq 1 ];
then
    TEST_7_RESULT=1
    echo "Test 7 Success"
else
    echo "Test 7 Fail"
fi


#Clean up (remove temp files and kill server)
kill $SERVER_PID
rm temp_file_test1
rm temp_file_test2
rm temp_file_test3
rm temp_file_test4
rm temp_file_test5
rm temp_file_test6
rm temp_file_test7

#Either exit 0 or exit 1 if tests pass or not
if [ $TEST_1_RESULT -eq 1 ] && [ $TEST_2_RESULT -eq 1 ] && [ $TEST_2_RESULT -eq 1 ] && \
   [ $TEST_4_RESULT -eq 1 ] && [ $TEST_5_RESULT -eq 1 ]&& [ $TEST_6_RESULT -eq 1 ] && \
   [ $TEST_7_RESULT -eq 1 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "One or more tests failed."
    exit 1
fi
