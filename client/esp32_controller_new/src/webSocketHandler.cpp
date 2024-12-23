#include "websocketHandler.h"

static bool webSocketConnected = false;

WebsocketsClient client;

void pingWebSocket()
{
    if (webSocketConnected)
    {
        client.ping();
    }
}

void pollWebSocket()
{
    if (webSocketConnected)
    {
        client.poll();
    }
}

void onMessageCallback(WebsocketsMessage message)
{
    Serial.print("Message received: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data)
{
    switch (event)
    {
    case WebsocketsEvent::ConnectionOpened:
        Serial.println("WebSocket connection established.");
        webSocketConnected = true;
        break;
    case WebsocketsEvent::ConnectionClosed:
        Serial.println("WebSocket connection closed. Attempting to reconnect...");
        webSocketConnected = false;
        break;
    case WebsocketsEvent::GotPing:
        Serial.println("Ping received.");
        webSocketConnected = true;
        break;
    case WebsocketsEvent::GotPong:
        Serial.println("Pong received.");
        webSocketConnected = true;
        break;
    }
}

void connectWebSocket(const String serverHost)
{
    Serial.println("Connecting to WebSocket...");

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);

    while (!webSocketConnected)
    {
        if (client.connect(serverHost))
        {
            Serial.println("WebSocket connected!");
            client.send("Hello Server");
            client.ping();

            webSocketConnected = true;
        }
        else
        {
            Serial.println("WebSocket connection failed. Retrying...");
            delay(1000); // Wait 1 second before retrying
        }
    }
}

void checkWebSocket(unsigned long interval)
{
    static unsigned long lastCheckTime = 0;

    if (millis() - lastCheckTime < interval)
    {
        return;
    }

    lastCheckTime = millis();

    if (!webSocketConnected || !client.available())
    {
        client.connect(Config::WEBSOCKETS_SERVER_HOST);
    }
    else
    {
        pingWebSocket();
        pollWebSocket();
    }
}