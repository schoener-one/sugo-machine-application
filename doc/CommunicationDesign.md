# Sugo Communication Design

The communication design is highly influenced by the used ZMQ library on which the messaging is based.

## Supported communication patterns

* Req-Rep: A request is sent from the client to the server in order to trigger an event or request a property.
Every request has to be replied by the server. Only after the reply has been received from the client, the next
request to the same server instance is possible.
* Pub-Sub: A notification can be published by a publisher. Every instance which is interested to a special notified
topic could subscribe to that topic and will receive it.

## Addressing

The addressing is based on the component service identifier (`ServiceId`). A service identifier always contains a service
address and a topic. Every component only knows the service identifier it is communicating to but never the full
qualified network address. Only the message broker knows full qualified network addresses of the responder.

Exemple service identifier:
```
<component-id>/<topic>
```

Regarding to the selected communication mode, e.g. _Inproc_ or _TCP_ the address scheme could change.

### Inproc addresses

The address of the responder service is as follows:

```
inproc://<component-id>/<topic>
```

If a publisher service is addressed the following address format is used:

```
inproc://<component-id>Pub/<topic>
```

### TCP addresses

```
tcp://<host-address>:<component-port>/<topic>
```

If the **TCP** communication is used, the message broker provides a translation between `component-id` and `component-port` number.

### Messaging

A message is always distinct by its service identifier `ServiceId`, which is a unique number (32bit) derived from the service component and the topic name (FNV1a hash). A unique number is more efficient to transmit and translate it on receiver side.

The whole information bunch of an component-id, topic and the message identifier is called `ServiceId`, because it describes exactly one service within the system.