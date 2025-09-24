FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash example_user

USER example_user

CMD ["bin/bash","-c","echo $USER >> usuario.txt"]