export BOOST_VERSION="1.83.0"
docker build -f Dockerfile.boost --build-arg BOOST_VERSION=${BOOST_VERSION} --tag=boost:${BOOST_VERSION} .