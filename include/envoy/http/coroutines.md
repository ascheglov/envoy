# Notes on coroutines

How to start it up:

- User code can make its own `Coroutine run();` and call it from a constructor.

- Or, filter base calss could have a `virtual Coroutine run() PURE;` and call it lazily from `decodeHeaders/etc`. (It cannot call virtual functions from constructor).

However it's not clear how a encoder-decoder would look like.

The `Coroutine` class is not an awaitable, and we only need a coroutine handle to restroy its state.


---------

`await_suspend` could do nothing it we already have the coroutine handle. However we could use a nested coroutine which would have a different handle.
