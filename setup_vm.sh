#!/bin/bash

cp -f vm_setup/app_config.h ../boards/buckler_revB/app_config.h
cp -f vm_setup/Board.mk ../boards/buckler_revB/Board.mk
cp -f vm_setup/Includes.mk ../nrf52x-base/make/Includes.mk

git config --global user.email 'seiya.ono12@gmail.com'
git config --global user.name 'onibrow'
alias jk='cd ..'
