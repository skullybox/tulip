#! /bin/bash

TARGET_DIR=../Android/Tulip/app/src/main/jniLibs

for i in $(ls Make_*); do make -f $i android CL=test; done

$(cp ./out/arm-linux-androideabi-4.9/*.so $TARGET_DIR/armeabi-v7a/)
$(cp ./out/arm-linux-androideabi-4.9/*.so $TARGET_DIR/armeabi/)
$(cp ./out/aarch64-linux-android-4.9/*.so $TARGET_DIR/arm64-v8a/)
$(cp ./out/mipsel-linux-android-4.9/*.so $TARGET_DIR/mips/)
$(cp ./out/mips64el-linux-android-4.9/*.so $TARGET_DIR/mips64/)
$(cp ./out/x86-4.9/*.so $TARGET_DIR/x86/)
$(cp ./out/x86_64-4.9/*.so $TARGET_DIR/x86_64/)

#for i in $(ls Make_* | cut -f2- -d"_"); do G=$(dirname `find ~ /Android/ndk-bundle/platforms/android-22/* -type f -name "*.so" -print | grep "$i" | head -1 `); echo "$G - $i"; done;
$(cp ./out/arm-linux-androideabi-4.9/*.so ~/Android/ndk-bundle/platforms/android-22/arch-arm/usr/lib)
$(cp ./out/aarch64-linux-android-4.9/*.so ~/Android/ndk-bundle/platforms/android-22/arch-arm64/usr/lib)
$(cp ./out/mipsel-linux-android-4.9/*.so ~/Android/ndk-bundle/platforms/android-22/arch-mips/usr/lib)
$(cp ./out/mips64el-linux-android-4.9/*.so ~/Android/ndk-bundle/platforms/android-22/arch-mips64/usr/lib64)
$(cp ./out/x86-4.9/*.so ~/Android/ndk-bundle/platforms/android-22/arch-x86/usr/lib)
$(cp ./out/x86_64-4.9/*.so ~/Android/ndk-bundle/platforms/android-22/arch-x86_64/usr/lib64)
