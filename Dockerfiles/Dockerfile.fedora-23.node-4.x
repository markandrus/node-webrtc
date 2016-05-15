FROM fedora:23

RUN curl --silent --location https://rpm.nodesource.com/setup_4.x | bash -
RUN yum -y install expat-devel gcc-c++ nodejs tar which
RUN yum -y groupinstall 'Development Tools' 'Development Libraries'

ADD ./ /wrtc/
WORKDIR /wrtc/

RUN npm install && node_modules/.bin/node-pre-gyp install --fallback-to-build
RUN npm install
RUN npm test
