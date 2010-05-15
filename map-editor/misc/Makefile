
TARGET=bombic-map-editor

all:
	pod2man --section=6 $(TARGET).pod > $(TARGET).6
	rm -f  $(TARGET).6.gz	
	gzip $(TARGET).6

clean:
	rm -f  $(TARGET).6*
