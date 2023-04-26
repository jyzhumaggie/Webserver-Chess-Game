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

#Test 2: testing a correct response through nc
echo -e "hello\n\n" | timeout $TIMEOUT nc $SERVER_IP $SERVER_PORT > temp_file_test2

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

#Clean up (remove temp files and kill server)
kill $SERVER_PID
rm temp_file_test1
rm temp_file_test2
rm temp_file_test3


#Either exit 0 or exit 1 if tests pass or not
if [ $TEST_1_RESULT -eq 1 ] && [ $TEST_2_RESULT -eq 1 ] && [ $TEST_2_RESULT -eq 1 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "One or more tests failed."
    exit 1
fi
