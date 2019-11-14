#!/bin/bash

cp -f vm_setup/app_config.h ../boards/buckler_revB/app_config.h
cp -f vm_setup/Board.mk ../boards/buckler_revB/Board.mk
cp -f vm_setup/mcp4725.c ~/buckler/software/nrf52x-base/sdk/nrf5_sdk_15.3.0/components/drivers_ext/mcp4725/mcp4725.c

git config --global user.email 'seiya.ono12@gmail.com'
git config --global user.name 'onibrow'
