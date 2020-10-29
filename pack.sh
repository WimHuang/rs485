#!/bin/bash


cp rs485_raw ../arm_buildroot/output/target/root
cp rs485_sensor ../arm_buildroot/output/target/root

cd ../arm_buildroot
make
cd -

cp ../arm_buildroot/output/images/rootfs.cpio.uboot ~/share/
