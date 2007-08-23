#
## Makefile.am for limal/testsuite
#
#SUBDIRS = lib config limal.test single.out callback_test1_lib callback_test1
PACKAGE = pam-config
AUTOMAKE_OPTIONS = dejagnu

clean-local:
	rm -f tmp.err.* tmp.out.* site.exp site.bak
	rm -f *~ pam-config.log pam-config.sum
	rm -rf single.out/out
	rm -f single.out/*~
	rm -f etc/*~
	rm -f etc/*/*~
	rm -f etc/pam.d/common-*-pc

EXTRA_DIST = single.out/*.* *.single etc/* etc/pam.d
