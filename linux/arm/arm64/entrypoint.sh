#!/usr/bin/env bash

groupadd -o -g $BUILDER_GID $BUILDER_GROUP 2> /dev/null
useradd -o -m -g $BUILDER_GID -u $BUILDER_UID $BUILDER_USER 2> /dev/null
export HOME=/home/${BUILDER_USER}

shopt -s dotglob
cp -r /root/* $HOME/
chown -R $BUILDER_UID:$BUILDER_GID $HOME
chown -R $BUILDER_UID:$BUILDER_GID /build

# Enable passwordless sudo capabilities for the user
chown root:$BUILDER_GID $(which gosu)
chmod +s $(which gosu); sync

# Run the command as the specified user/group.
exec gosu $BUILDER_UID:$BUILDER_GID "$@"
