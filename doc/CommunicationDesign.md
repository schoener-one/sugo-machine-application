# Sugo Communication Design

The communication design is highly influenced by the used ZMQ library on which the messaging is based.

## Supported communication patterns

* Req-Rep: A request is sent from the client to the server in order to trigger an event or request a property.
Every request has to be replied by the server. Only after the reply has been received from the client, the next
request to the same server instance is possible.
* Pub-Sub: A notification can be published by a publisher. Every instance which is interested to a special notified
topic could subscribe to that topic and will receive it.

## Addressing

There are to two addressing types regarding to the selected communication system.

### Inproc addresses

```
inproc://<component>.<com-type>[/<topic>]
```

The com-type could be `srv`, `pub` or `sub`.


### TCP addresses

```
tcp://<host>:<component-port>[/<topic>]
```