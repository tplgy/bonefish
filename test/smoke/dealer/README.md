Smoke Test: Dealer
==================

## Description

The caller and callee found here are meant to be used as a manual
smoke test to put bonefish under stress in order to determine if
there are any issues in the data path for the dealer.

Terminal 1:
```
$ <bonefish>
.
.
.
```

Terminal 2:
```
$ python callee.py
.
.
.
```

Terminal 3:
```
$ python caller.py
.
.
.
```
