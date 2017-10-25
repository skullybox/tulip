#! /bin/bash

TARGET_DIR=../Android/Tulip/app/src/main/jniLibs

for i in $(ls Make_*); do make -f $i android CL=test; done

$(cp ./out/arm-linux-androideabi-4.9/*.so $TARGET_DIR/armeabi-v7a/)
$(cp ./out/arm-linux-androideabi-4.9/*.so $TARGET_DIR/armeabi/)
$(cp ./out/aarch64-linux-android-4.9/*.so $TARGET_DIR/arm64_v8a/)
$(cp ./out/mipsel-linux-android-4.9/*.so $TARGET_DIR/mips/)
$(cp ./out/mips64el-linux-android-4.9/*.so $TARGET_DIR/mips64/)
$(cp ./out/x86-4.9/*.so $TARGET_DIR/x86/)
$(cp ./out/x86_64-4.9/*.so $TARGET_DIR/x86_64/)
