FROM ubuntu:22.04

RUN apt-get update && apt-get upgrade -y

RUN apt-get install python3 python3-pip -y

COPY ejemplo_python /ejemplo_python

COPY ejemplo_python/ejemplo_python.py /ejemplo_python.py

# WORKDIR /ejemplo_python

CMD ["python3","ejemplo_python.py"]

