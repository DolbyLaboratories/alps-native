########################################################################################################################
# Copyright (C) 2024 by Dolby International AB.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
# following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
#    disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
#    products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
########################################################################################################################

FROM ubuntu:24.04

# install required packages
RUN <<EOF
    apt-get update
    DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y build-essential git cmake doxygen pipx curl zip
    pipx install conan
EOF
ENV PATH="$PATH:/root/.local/bin"

# install Android NDK
RUN apt-get install -y google-android-ndk-r25b-installer

# create Conan profiles for Android
COPY <<EOF /root/.conan2/profiles/android-armv7
[settings]
os=Android
os.api_level=29
arch=armv7
compiler=clang
compiler.version=14
compiler.libcxx=c++_static
compiler.cppstd=14
build_type=Release

[conf]
tools.android:ndk_path=/usr/lib/android-sdk/ndk/25.1.8937393
EOF
COPY <<EOF /root/.conan2/profiles/android-armv8
[settings]
os=Android
os.api_level=29
arch=armv8
compiler=clang
compiler.version=14
compiler.libcxx=c++_static
compiler.cppstd=14
build_type=Release

[conf]
tools.android:ndk_path=/usr/lib/android-sdk/ndk/25.1.8937393
EOF
# create default Conan profile and install code dependencies
RUN <<EOF
conan profile detect
conan install --profile=default --requires=cmocka/1.1.7 --build=missing
EOF

# install GitLab's release-cli for creating GitLab releases
RUN <<EOF
curl --location --output /usr/local/bin/release-cli "https://gitlab.com/api/v4/projects/gitlab-org%2Frelease-cli/packages/generic/release-cli/latest/release-cli-linux-amd64"
chmod +x /usr/local/bin/release-cli
EOF

# install and configure JFrog CLI
RUN <<EOF
mkdir -p /usr/share/keyrings;
wget -qO - https://releases.jfrog.io/artifactory/jfrog-gpg-public/jfrog_public_gpg.key | gpg --dearmor -o /usr/share/keyrings/jfrog.gpg;
echo "deb [signed-by=/usr/share/keyrings/jfrog.gpg] https://releases.jfrog.io/artifactory/jfrog-debs xenial contrib" | tee /etc/apt/sources.list.d/jfrog.list;
apt update;
apt install -y jfrog-cli-v2-jf;
EOF


WORKDIR /usr/src/build
