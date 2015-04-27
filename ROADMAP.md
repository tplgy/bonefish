## Higher Priority
- add approriate system/debug logging facilities
    - explore a non-boost based approach
        - the boost logging library is very bloated
        - just need something simple to just use for debugging
            - the debugging framework in libnl was nice and simple

## Lower Priority
- add appropriate json based daemon configuration
    - this would allow for specifying things like
        - realm configuration
        - transport configuration
        - router configuration
        - hardware concurreny
- add proper uri validation according to the spec
- move uri checking out of the serialization
    - allow each broker/dealer/router to perform uri validation so that the appropriate error messages can be generated where applicable.
- add support for publish options so that publish acknowledgements can be used
- enumerate wampv2 subprotocol to eliminate string comparisons
- split component and router roles into two different types

## Performance
- revisit the data path that messages follow and eliminate any unnecessary copies
- take a look at how we can use multiple cores more effectively
- vertical scalability
    - option 1: multiple message queues/threads per router (lockless)
    - option 2: multiple router instances per realm
    - for now we are running lean with everything running on a single thread
