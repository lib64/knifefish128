SUBDIRS := pht block invert_ctx expand_passphrase encrypt_file_cbc

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

clean:
	$(MAKE) -C pht clean
	$(MAKE) -C block clean
	$(MAKE) -C invert_ctx clean
	$(MAKE) -C expand_passphrase clean
	$(MAKE) -C encrypt_file_cbc clean
