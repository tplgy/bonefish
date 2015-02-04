###############################################################################
##
##  Copyright (C) 2014 Tavendo GmbH
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.
##
###############################################################################

import sys
import six
import datetime

from twisted.python import log
from twisted.internet import reactor
from twisted.internet.defer import inlineCallbacks
from twisted.internet.endpoints import clientFromString

from autobahn.twisted import wamp, websocket
from autobahn.wamp import types

class MyFrontendComponent(wamp.ApplicationSession):
   """
   Application code goes here. This is an example component that provides
   a simple procedure which can be called remotely from any WAMP peer.
   It also publishes an event every second to some topic.
   """

   @inlineCallbacks
   def onJoin(self, details):
      ## register a procedure for remote calling
      ##
      def utcnow():
         print("Someone is calling me;)")
         now = datetime.datetime.utcnow()
         return six.u(now.strftime("%Y-%m-%dT%H:%M:%SZ"))

      reg = yield self.register(utcnow, u'com.timeservice.now')
      print("Registered procedure with ID {}".format(reg.id))

if __name__ == '__main__':
   ## 0) start logging to console
   log.startLogging(sys.stdout)

   ## 1) create a WAMP application session factory
   component_config = types.ComponentConfig(realm = "realm1")
   session_factory = wamp.ApplicationSessionFactory(config = component_config)
   session_factory.session = MyFrontendComponent

   ## optional: use specific set of serializers
   if False:
      serializers = None
   else:
      from autobahn.wamp.serializer import *
      serializers = []
      #serializers.append(JsonSerializer(batched = True))
      #serializers.append(MsgPackSerializer(batched = True))
      #serializers.append(JsonSerializer())
      serializers.append(MsgPackSerializer())

   ## 2) create a WAMP-over-WebSocket transport client factory
   transport_factory = websocket.WampWebSocketClientFactory(session_factory,
      serializers = serializers, debug = False, debug_wamp = True)

   ## 3) start the client from a Twisted endpoint
   client = clientFromString(reactor, "tcp:127.0.0.1:9005")
   client.connect(transport_factory)

   ## 4) now enter the Twisted reactor loop
   reactor.run()
