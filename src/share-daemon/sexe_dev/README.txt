
( api )
/** Used in order to obtain a persistent storage object. */
sdev_data([inode])

/** Called by the device driver during a callback proc to state an error occurred. */
sdev_error([inode])


( callback )
/** Called in the device driver when the device is initialized. */
sdev_init()

/** Called in the device driver when the device is terminated. */
sdev_term()

/** Called when the device is opened for I/O by a peer. */
sdev_open(peer, inode, is_async)

/** Called when the device I/O is finished for a peer. */
sdev_release(peer, inode)

/** Called when a read operation is requested by the user. */
sdev_read(inode, int len)

/** Called when a write operation is requested by the user. */
sdev_write(inode, void *data, int len)

/** Called when a seek operation is requested by the user. */
sdev_seek(inode, int offset)

/** Called in order to pass control parameters to/from the device driver. */
sdev_ctl(int arg, void *data)

/** Called when the device is requested to be exclusive. */
sdev_lock(peer, inode)

/** Called when the device is no longer desired to be exclusive. */
sdev_unlock(peer, inode)

/** Called when a device data flush is desired. */
sdev_flush(inode)

/** Called in order to determine whether data is available for read or write. */
sdev_poll(inode, int timeout_ms)

/** Called once per second, per inode device open, for continous operations to be performed. */
sdev_timer(inode)


