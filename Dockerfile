FROM --platform=linux/amd64 alpine:3.20.1

WORKDIR /app

COPY . .

# Instala GCC e make
RUN apk add --no-cache gcc musl-dev make

RUN sh build.sh

CMD ["/app/test_libcsv"]
