#    Makefile for the Arduino programm for NWT
#    Copyright (C) 2020  Johannes Schmatz
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# please config before use
# <config>
PORT="/dev/ttyUSB0"

## source files

# old varinat
#FILE="DISCO/DISCO.ino"
# new variant
FILE="DISCO"

CLIENT_FILE="client/main.c"
CLIENT_O_FILE="output/client"

## compilers
# C-compiler gcc
COMPILER="gcc"

# C-compiler cc
#COMPILER="cc"

## pagers
# pager less
PAGER="less"

# pager more
#PAGER="more"

## editors
# editor vim
EDITOR="vim"
EDITOR_RC_FILE="/home/johannes/.vimrc"
SYNTAX_ADD_FILE="vimrc_add.txt"

# editor nano
#EDITOR="nano"
#EDITOR_RC_FILE="/dev/null"
#SYNTAX_ADD_FILE="vimrc_add.txt"

# </config>

# old variant
#UPLOADER="arduino --upload"
#UPLOADER_UP="--port"
# nwe variant
UPLOADER=arduino-cli compile -b arduino:avr:uno -up
UPLOADER_UP=

# make all (upload, installing syntax highlighting, printing the readme, 
all: install-syntax-highlighting install

install-syntax-highlighting:
	bash -c 'grep -v // $(SYNTAX_ADD_FILE) >> $(EDITOR_RC_FILE)'

install: config upload install-client
	@echo Done.

config: makefile-edit
	@echo Please config the Makefile first, then the programm will be
	@echo compiled and uploaded to the Arduino, on the port, that you 
	@echo config in the Makefile. Also add your favourite pager and
	@echo editor. You might want them on the same values as '$$EDITOR'
	@echo or '$$PAGER'. Also add the editors RC file and a syntax
	@echo adding file for *.ino files \(if not installed set the RC file
	@echo as '/dev/null'\). Press ENTER to edit the Makefile.
	@echo 
	@echo This programm and its components are Free Software under GPLv3.
	@read var

makefile-edit:
	@$(EDITOR) Makefile

upload:
	$(UPLOADER) $(PORT) $(UPLOADER_UP) $(FILE) 

install-client:
	$(COMPILER) -o $(CLIENT_O_FILE) $(CLIENT_FILE)

arduino-edit:
	@$(EDITOR) $(FILE)

client-edit:
	@$(EDITOR) $(CLIENT_FILE)
	$(COMPILER) -o $(CLIENT_O_FILE) $(CLIENT_FILE)

edit: arduino-edit upload-pager

upload-pager:
	$(UPLOADER) $(PORT) $(UPLAODER_UP) $(FILE)  2>&1 | $(PAGER)

serial: 
	@echo Arguments:
	@echo ' s - song'
	@echo ' l - leds'
	@echo ' n - noleds song'
	@echo 
	@echo -n 'please type in your arguments: '
	@read vars; $(CLIENT_O_FILE) $(PORT) -$$vars
	
# convert tabs to spaces
convert:
	@./sed_tab_to_space.sh DISCO/DISCO.ino DISCO/DISCO.spaces_not_tab.ino
	@./sed_space_to_tab.sh DISCO/DISCO.ino DISCO/DISCO.tab_not_spaces.ino


#	@echo use 'C-a + k' to kill the window \(in screen\)
#	@read var
#	@screen $(PORT)

serial-direct:
	@screen $(PORT)
