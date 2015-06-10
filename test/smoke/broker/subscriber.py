import sys

from twisted.python import log
from twisted.internet import reactor
from twisted.internet.defer import inlineCallbacks
from twisted.internet.endpoints import clientFromString

from autobahn.twisted import wamp, websocket
from autobahn.wamp import types
from autobahn.wamp.serializer import *

class Component(wamp.ApplicationSession):
    """
    Component that joins the router and subscribes to receive
    large random length strings on the topic:

          io.bonefish.test.random_data

    See README.md
    """

    @inlineCallbacks
    def onJoin(self, details):
        sub = yield self.subscribe(self.onRandomDataEvent, u'io.bonefish.test.random_data')
        print("Subscription id: {}".format(sub.id))

    @inlineCallbacks
    def onDisconnect(self):
        reactor.stop()

    def onRandomDataEvent(self, i):
        print("Random data event: {}".format(i))

if __name__ == '__main__':
    # Start logging to console
    log.startLogging(sys.stdout)

    ## Create an application session factory
    component_config = types.ComponentConfig(realm = "default")
    session_factory = wamp.ApplicationSessionFactory(config = component_config)
    session_factory.session = Component

    # Setup the serializers that we can support
    serializers = []
    #serializers.append(JsonSerializer(batched = True))
    #serializers.append(MsgPackSerializer(batched = True))
    #serializers.append(JsonSerializer())
    serializers.append(MsgPackSerializer())

    # Create a websocket transport factory
    transport_factory = websocket.WampWebSocketClientFactory(session_factory,
        serializers = serializers, debug = False, debug_wamp = True)

    ## Start the client and connect
    client = clientFromString(reactor, "tcp:127.0.0.1:8001")
    client.connect(transport_factory)

    # Run the reactor loop
    reactor.run()
