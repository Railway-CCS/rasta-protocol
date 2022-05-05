FROM debian:11
COPY build/bin/ /workspaces/rasta-bridge/build/bin/
COPY entrypoint .
EXPOSE 50051
ENTRYPOINT ["/entrypoint"]
