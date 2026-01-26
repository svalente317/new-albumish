all: albumish.exe

ALBUMISH_FILES = src/albumish/*.san src/database/*.san src/filter/*.san \
		 src/sync/*.san src/dialogs/*.san

GTK_FILES = src/gtk/*.san src/gdk/*.san
GTK_CONFIG = --pkg-config gtk+-3.0

AUDIO_FILES = src/audio/*.san src/audio/miniaudio.h src/audio/miniaudio.c
AUDIO_CONFIG = --pkg-config libmpg123

albumish.exe: $(ALBUMISH_FILES) bin/gtk.sanka.tar bin/audio.sanka.tar
	sanka --top bin $(GTK_CONFIG) $(AUDIO_CONFIG) $(ALBUMISH_FILES) \
	--lib bin/gtk.sanka.tar --lib bin/audio.sanka.tar --main albumish.Albumish --exe $@

bin/gtk.sanka.tar: $(GTK_FILES)
	sanka --top bin $(GTK_CONFIG) $(GTK_FILES) --create-library $@

bin/audio.sanka.tar: $(AUDIO_FILES)
	sanka --top bin $(AUDIO_CONFIG) $(AUDIO_FILES) --create-library $@

src/audio/miniaudio.h:
	curl https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h -o $@

clean:
	rm -rf albumish.exe bin
