import datetime
import random
import six
import string
import sys

from twisted.python import log
from twisted.internet import reactor
from twisted.internet.defer import inlineCallbacks
from twisted.internet.endpoints import clientFromString

from autobahn.twisted import wamp, websocket
from autobahn.wamp import types
from autobahn.wamp.exception import ApplicationError
from autobahn.wamp.serializer import *

class Component(wamp.ApplicationSession):
    """
    Component that joins the router and registers a procedure
    that returns large amounts of random length data to the
    caller:

          io.bonefish.test.get_getRandomData

    See README.md
    """

    @inlineCallbacks
    def onJoin(self, details):
        reg = yield self.register(self.getRandomData, u'io.bonefish.test.get_random_data')
        print("Registered procedure with ID {}".format(reg.id))

    @inlineCallbacks
    def onDisconnect(self):
        reactor.stop()

    def getRandomData(self):
        chars = string.ascii_uppercase + string.digits
        upper_bound = random.randint(1,131072) % 131072
        return ''.join(random.choice(chars) for _ in range(upper_bound))

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
        serializers = serializers, debug = True, debug_wamp = True)

    ## Start the client and connect
    client = clientFromString(reactor, "tcp:127.0.0.1:8001")
    client.connect(transport_factory)

    # Run the reactor loop
    reactor.run()
