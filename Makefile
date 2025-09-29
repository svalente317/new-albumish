all: albumish

ALBUMISH_FILES = src/audio/*.san src/albumish/*.san src/database/*.san src/filter/*.san src/gtk/*.san

PKG_CONFIG = --pkg-config gtk+-3.0 --pkg-config libmpg123 --pkg-config libpulse-simple

albumish: $(ALBUMISH_FILES)
	sanka --top bin $(PKG_CONFIG) $(ALBUMISH_FILES) --main albumish.Albumish --exe $@

bin/audio.sanka.tar: src/audio/*.san
	sanka src/audio/*.san --create-library $@

HELLO_FILES = src/main/Hello.san src/gtk/*.san

hello: $(HELLO_FILES)
	sanka --top bin --pkg-config gtk+-3.0 $(HELLO_FILES) --main main.Hello --exe $@

MP3_FILES = src/main/Mp3Play.san src/audio/*.san

MP3_PKGCONFIG = --pkg-config libmpg123 --pkg-config libpulse-simple

mp3play: $(MP3_FILES)
	sanka --top bin $(MP3_PKGCONFIG) $(MP3_FILES) --main main.Mp3Play --exe $@

clean:
	rm -rf albumish hello mp3play finder bin
