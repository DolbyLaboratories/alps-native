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
