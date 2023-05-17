FROM python:alpine

RUN python -m pip install requests
RUN python -m pip install requests-toolbelt

RUN mkdir -p /script

COPY script.py /script

WORKDIR /workdir

CMD ["python3", "/script/script.py"]
