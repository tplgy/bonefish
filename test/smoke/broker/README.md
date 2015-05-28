Smoke Test: Broker
==================

## Description

The publisher and subscriber found here are meant to be used
as a manual smoke test to put bonefish under stress in order
to determine if there are any issues in the data path for the
broker.

Terminal 1:
```
$ <bonefish>
.
.
.
```

Terminal 2:
```
$ python subscriber.py
.
.
.
```

Terminal 3:
```
$ python publisher.py
.
.
.
```

```
