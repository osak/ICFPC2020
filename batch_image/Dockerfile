FROM docker.pkg.github.com/osak/dev-images/rust:latest

USER root

# Place deploy_key.pem in the same directory to this Dockerfile. It must not be committed to Git.
COPY deploy_key.pem /root/.ssh/deploy_key.pem
COPY ssh_config /root/.ssh/config
COPY known_hosts /root/.ssh/known_hosts
RUN chmod -R 700 /root/.ssh

COPY run.sh /

ENTRYPOINT ["/bin/bash", "run.sh"]
