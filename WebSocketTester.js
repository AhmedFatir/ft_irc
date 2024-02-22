//###########################################################//

//script to connect with Internet Relay Chat websocket tester//

//###########################################################//



const WebSocket = require('ws');
const readline = require('readline');

const username = process.argv[2];
if (!username) {
  console.error('Error: usage (node WebSocketTester.js <username>)');
  process.exit(1);
}

const socket = new WebSocket('wss://testnet.ergo.chat/webirc', ['text.ircv3.net', 'binary.ircv3.net']);

// Event listener for WebSocket 'open' event
socket.on('open', () => {
  console.log('Connected to the server');
  // Send initial messages to the server
  socket.send(`NICK ${username}`);
  socket.send('USER 0 0 0 0');
});

// Event listener for WebSocket 'message' event
socket.on('message', async (data) => {
  // Convert the binary data to a string
  const message = data.toString('utf8');
  console.log('Received message from server:', message);
});

// Event listener for WebSocket 'error' event
socket.on('error', (error) => {
  console.error('WebSocket error:', error);
});

// Create interface for reading input from the terminal
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

// Function to handle sending messages
function sendMessage(message) {
  socket.send(message);
}

// Event listener for terminal input
rl.on('line', (input) => {
  // Send the input as a message to the server
  sendMessage(input);
});
