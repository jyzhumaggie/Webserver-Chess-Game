#!/bin/bash

#run the webserver
../build/bin/webserver ../config/crud_integration_test.conf &
mkdir resources

#server info
SERVER_PID=$!
SERVER_IP=localhost
SERVER_PORT=900

#Make sure server is fully running
sleep 1

#Test 1: Creating and retrieving entity
curl -sSH "Content-Type: application/json" \
    -d '{"test_data":"test"}' -X POST "${SERVER_IP}:${SERVER_PORT}/resource/Tests"

curl -sSo temp_file_test1 "${SERVER_IP}:${SERVER_PORT}/resource/Tests/1"

diff temp_file_test1 crud_integration_test/expected_1

RESULT=$?

TEST_1_RESULT=0

if [ $? -eq 0 ];
then
   TEST_1_RESULT=1
   echo "Test 1 Success"
else
   echo "Test 1 Fail"
fi

#Test 2: Restart server and make sure retrieve still works
kill $SERVER_PID
../build/bin/webserver ../config/crud_integration_test.conf &

SERVER_PID=$!
sleep 1

curl -sSo temp_file_test2 "${SERVER_IP}:${SERVER_PORT}/resource/Tests/1"
diff temp_file_test2 crud_integration_test/expected_1

RESULT=$?

TEST_2_RESULT=0

if [ $RESULT -eq 0 ];
then
   TEST_2_RESULT=1
   echo "Test 2 Success"
else
   echo "Test 2 Fail"
fi

#Test 3: Delete resource and make sure retrieve stops working
curl -sS -X DELETE "${SERVER_IP}:${SERVER_PORT}/resource/Tests/1"
curl -sSo temp_file_test3 "${SERVER_IP}:${SERVER_PORT}/resource/Tests/1"

diff temp_file_test3 crud_integration_test/expected_2

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
rm -rf resources
rm temp_file_test1
rm temp_file_test2
rm temp_file_test3

#Either exit 0 or exit 1 if tests pass or not
if [ $TEST_1_RESULT -eq 1 ] && [ $TEST_2_RESULT -eq 1 ] && [ $TEST_3_RESULT -eq 1 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "One or more tests failed."
    exit 1
fi
