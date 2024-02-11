SERVER="localhost"  
PORT=9009      
CHANNEL="#Room"
CLIENT_COUNT=500
PASS="123"

for ((i=1; i<=CLIENT_COUNT; i++))
do
    (
    NICKNAME="Testuser${i}"
    echo "PASS $PASS"
    sleep 0.25
    echo "NICK $NICKNAME"
    sleep 0.25
    echo "USER ${i} 0 * abdo"
    sleep 0.25
    echo "JOIN $CHANNEL"
    sleep 500
    ) | nc $SERVER $PORT &
    sleep 0.5
done
wait 
echo "Disconnected all clients from the server."