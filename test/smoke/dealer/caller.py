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
    Component that joins the router and consumes large
    amounts of random length data provided by the callee:

          io.bonefish.test.get_random_data

    See README.md
    """

    @inlineCallbacks
    def onJoin(self, details):
        yield reactor.callLater(0, self.getRandomData)

    @inlineCallbacks
    def onDisconnect(self):
        reactor.stop()

    @inlineCallbacks
    def getRandomData(self):
        try:
            data = yield self.call(u'io.bonefish.test.get_random_data')
        except Exception as e:
            print("Error: {}".format(e))
        else:
            print("Random data: %s" % data)
        yield reactor.callLater(0, self.getRandomData)

if __name__ == '__main__':
    # Start logging to console
    log.startLogging(sys.stdout)

    # Create an application session factory
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

    # Start the client and connect
    client = clientFromString(reactor, "tcp:127.0.0.1:8001")
    client.connect(transport_factory)

    # Run the reactor loop
    reactor.run()
