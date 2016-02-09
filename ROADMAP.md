ROADMAP
=======

## Basic Profile

The following items need to be completed before we can officially say that the basic profile is fully supported. Refer to [SPECIFICATION.md](SPECIFICATION.md) for details on the basic profile.

### Publishing

Add support for specifying acknowledgements for publishing:

```
PUBLISH.Options.acknowledge
```

## Advanced Profile

The advanced profile is fairly extensive and is very feature based. There will be an ongoing effort to implement these features on an ongoing basis. We will call out each feature below as they are supported:

<table>
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
<td>Allows callers to specify a timeout. The default is never timeout.</td>
</tr>
<tr>
<td></td>
<td>Progressive Call Results</td>
<td>Allows callers to specify that they support receiving results that span across multiple yields from the callee. These multi-part style results are guaranteed to arrive in order.</td>
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
option 1: multiple message queues/threads per router (lockless)
    - option 2: multiple router instances per realm
    - for now we are running lean with everything running on a single thread

## Testing

### Unit Testing

We need to select a suitable unit testing framework to use with bonefish. I started looking into Aeryn which looks like it would be a great fit however a more comprehensive analysis should be done before selecting a framework.

### Automated testing

It should not be very dificult to write a handful of test components that can be used to exercise bonefish and verify its behaviour. A good starting point here would simply be a stress test that could be used to check for things like memory leaks, memory corruption, crashes, etc.

