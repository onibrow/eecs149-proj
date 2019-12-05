#!/bin/bash

rm ~/buckler/software/boards/buckler_revB/app_config.h
ln vm_setup/app_config.h ~/buckler/software/boards/buckler_revB/app_config.h

rm ~/buckler/software/boards/buckler_revB/Board.mk
ln vm_setup/Board.mk ~/buckler/software/boards/buckler_revB/Board.mk

rm ~/buckler/software/nrf52x-base/make/Includes.mk
ln vm_setup/Includes.mk ~/buckler/software/nrf52x-base/make/Includes.mk

rm ~/buckler/software/nrf52x-base/lib/simple_logger/simple_logger.c
ln vm_setup/simple_logger.c ~/buckler/software/nrf52x-base/lib/simple_logger/simple_logger.c

rm ~/buckler/software/nrf52x-base/lib/simple_logger/simple_logger.h
ln vm_setup/simple_logger.h ~/buckler/software/nrf52x-base/lib/simple_logger/simple_logger.h

git config --global user.email 'seiya.ono12@gmail.com'
git config --global user.name 'onibrow'
alias jk='cd ..'
