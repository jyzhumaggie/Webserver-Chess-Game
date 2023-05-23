#!/bin/bash

#run the webserver
../build/bin/webserver ../config/crud_integration_test.conf &
mkdir resources

#server info
SERVER_PID=$!
SERVER_IP=localhost
SERVER_PORT=900

#timeout variable
TIMEOUT=0.2

#Make sure server is fully running
sleep 1

#Test 1: Creating and retrieving entity
curl -sSH "Content-Type: application/json" \
    -d '{"test_data":"test"}' -X POST "${SERVER_IP}:${SERVER_PORT}/resource/Tests"

curl -sSo temp_file_test1 "${SERVER_IP}:${SERVER_PORT}/resource/Tests/1"

diff temp_file_test1 crud_integration_test/expected_1

TEST_1_RESULT=0

RESULT=$?

if [ $RESULT -eq 0 ];
then
   TEST_1_RESULT=1
   echo "Test 1 Success"
else
   echo "Test 1 Fail"
fi

#Clean up (remove temp files and kill server)
kill $SERVER_PID
rm -rf resources
rm temp_file_test1

#Either exit 0 or exit 1 if tests pass or not
if [ $TEST_1_RESULT -eq 1 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "One or more tests failed."
    exit 1
fi
