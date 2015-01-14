import sys

from twisted.python import log
from twisted.internet import reactor
from twisted.internet.defer import inlineCallbacks
from twisted.internet.endpoints import clientFromString

from autobahn.twisted import wamp, websocket
from autobahn.wamp import types

from autobahn.twisted.wamp import ApplicationSession, ApplicationRunner

class MyComponent(ApplicationSession):

   def onJoin(self, details):
      print("session joined")

##if __name__ == '__main__':
##   runner = ApplicationRunner(url = u"ws://localhost:9005/ws", realm = u"realm1")
##   runner.run(MyComponent)

if __name__ == '__main__':

   ## 0) start logging to console
   log.startLogging(sys.stdout)

   ## 1) create a WAMP application session factory
   component_config = types.ComponentConfig(realm = "realm1")
   session_factory = wamp.ApplicationSessionFactory(config = component_config)
   session_factory.session = MyComponent

   ## optional: use specific set of serializers
   if False:
      serializers = None
   else:
      from autobahn.wamp.serializer import *
      serializers = []
      #serializers.append(JsonSerializer(batched = True))
      #serializers.append(MsgPackSerializer(batched = True))
      #serializers.append(JsonSerializer())
      serializers.append(MsgPackSerializer(batched = False))

   ## 2) create a WAMP-over-WebSocket transport client factory
   transport_factory = websocket.WampWebSocketClientFactory(session_factory,
      serializers = serializers, debug = False, debug_wamp = False)

   ## 3) start the client from a Twisted endpoint
   client = clientFromString(reactor, "tcp:127.0.0.1:9005")
   client.connect(transport_factory)

   ## 4) now enter the Twisted reactor loop
   reactor.run()

