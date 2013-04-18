/**
* Copyright (C) 2013 Andrew Rifken
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute,
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
* NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/


#import <Foundation/Foundation.h>

@protocol ServerConnectionDelegate;

@class GCDAsyncSocket;
@class Message;
@class CLLocation;
@class Client;

typedef enum {
    ChatConnectionStateDisconnected = 0,
    ChatConnectionStateConnectedSigningIn,
    ChatConnectionStateSignedIn
} ChatConnectionState;


/**
* ServerConnection
*
* This is the interface between Cocoa and Node.js. It utilizes Robbie Hanson's CocoaAsyncSocket to stream data to and
* from the server. Each message sent and received is JSON encoded, and can be identified by the following "actions"
*
*   ACTION_CONNECTED     Sent by the server when a client has connected
*   ACTION_DISCONNECTED  Sent by the server when a client has disconnected
*   ACTION_MESSAGE = @"msg" a chat message was submitted
*   ACTION_LOCATION_REQUEST = @"loc_req"; // someone is requesting our current location
*   ACTION_LOCATION_RESPONSE = @"loc_res"; // someone is broadcasting their location
*   ACTION_HEARTBEAT= @"hb"; // the server is checking to make sure we are still connected
*
*/
@interface ServerConnection : NSObject {
    NSMutableArray *_connectedClients;
}

@property(copy) NSString *clientId; // The current device's client ID. This is set when the user signs into the app
@property(strong) GCDAsyncSocket *socket;
@property(weak) id <ServerConnectionDelegate> delegate;
@property ChatConnectionState connectionState;

/**
* Array of Client objects representing the different connected users
*/
- (NSArray *)connectedClients;

/**
* Open a connection to the server (the app will automatically attempt sign in once the socket is open)
*/
- (void)connect;

/**
* Disconnect from the server (close the socket, clear session)
*/
- (void)disconnect;

/**
* Send a message to the server. A Message can represent any number of actions (listed above)
*/
- (void)send:(Message *)message;

/**
* Convenience method for sending a ACTION_LOCATION_RESPONSE message to the server (broadcasting your device's location
* to the rest of the connected clients)
*/
- (void)sendLocation:(CLLocation *)location;

/**
* Sends a ACTION_LOCATION_REQUEST to the server for a given client. Upon receipt, that client will broadcast an
* ACTION_LOCATION_RESPONSE with its current location
*/
- (void)requestLocationForClientWithID:(NSString *)clientId;

/**
* Convenience method returning this device's Client object
*/
- (Client *)myClient;

/**
* Find a connected client by its client ID
*/
- (Client *)clientForID:(NSString *)clientID;
@end


/**
* ServerConnectionDelegate
*
* Callback target for the ServerConnection class
*/
@protocol ServerConnectionDelegate

/**
* Allows the delegate to respond to various errors generated by this class
*/
- (void)chatConnnection:(ServerConnection *)conn didReceiveError:(NSError *)error;

/**
* Called when a user submits a chat message to the thread
*/
- (void)chatConnection:(ServerConnection *)conn didReceiveChatMessage:(Message *)message;

/**
* Called when a user posts an update to their current location
*/
- (void)chatConnection:(ServerConnection *)conn didReceiveLocation:(CLLocation *)loc forClientID:(NSString *)clientId;

/**
* Called when a new user connects to the server
*/
- (void)chatConnection:(ServerConnection *)conn clientDidConnect:(Client *)client;

/**
* Called when a user disconnects from the server
*/
- (void)chatConnection:(ServerConnection *)conn clientDidDisconnect:(NSString *)clientId;

/**
* Accessor method to get the current device location to send to other users when requested
*/
- (CLLocation *)chatConnectionCurrentLocation:(ServerConnection *)conn;
@end