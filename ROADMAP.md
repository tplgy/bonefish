ROADMAP
=======

<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;}
.tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
.tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}
.tg .tg-9vto{font-family:Verdana, Geneva, sans-serif !important;}
</style>

## Basic Profile

The following items need to be completed before we can officially say that the basic profile is fully supported. Refer to _SPECIFICATION.md_ for details on the basic profile.

### Publishing

Add support for specifying acknowledgements for publishing:

```
PUBLISH.Options.acknowledge


## Advanced Profile

The advanced profile is fairly extensive and is very feature based. There will be an ongoing effort to implement these features on an ongoing basis. We will call out each feature below as they are supported:

<table class="tg">
  <tr>
    <th>Category</th>
    <th>Feature</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>Transports</td>
    <td>Rawsocket</td>
    <td>Enables the use of stream based sockets as WAMP application server endpoints.</td>
  </tr>
  <tr>
    <td>Remote Procedure Calls</td>
    <td>Call Timeouts</td>
    <td>Allows calls to specify a timeout. The deafult is never timeout.</td>
  </tr>
</table>

## Bonefish Misc

- add appropriate json based daemon configuration
    - this would allow for specifying things like
        - realm configuration
        - transport configuration
        - router configuration
        - hardware concurreny
- enumerate wampv2 subprotocol to eliminate string comparisons
- split component and router roles into two different types
- move all uri validation into the dealer/broker so the appropriate error messages can be generated where applicable

## Performance

- revisit the data path that messages follow and eliminate any unnecessary copies
- take a look at how we can use multiple cores more effectively
- vertical scalability
    - option 1: multiple message queues/threads per router (lockless)
    - option 2: multiple router instances per realm
    - for now we are running lean with everything running on a single thread

## Testing

### Unit Testing

We need to select a suitable unit testing framework to use with bonefish. I started looking into Aeryn which looks like it would be a great fit however a more comprehensive analysis should be done before selecting a framework.

### Component Testing

We need to get rid of the python based test components and use autobahn-cpp instead. This should also help to drive development of autobahn-cpp.

### Automated testing

It should not be very dificult to write a handful of test components that can be used to exercise bonefish and verify its behaviour. A good starting point here would simply be a stress test that could be used to check for things like memory leaks, memory corruption, crashes, etc.

