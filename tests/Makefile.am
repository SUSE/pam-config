#
## Makefile.am for limal/testsuite
#
# SUBDIRS = single.out support testcases 
PACKAGE = pam-config
AUTOMAKE_OPTIONS = dejagnu

clean-local:
	rm -f tmp.err.* tmp.out.* site.exp site.bak
	rm -f *~ pam-config.log pam-config.sum
	rm -rf single.out/out
	rm -f single.out/*~
	rm -f etc/*~
	rm -f etc/*/*~
	rm -f etc/pam.d/common-*

EXTRA_DIST =  etc/* etc/pam.d/*.* support/*.* testcases/*.single single.out/*.* \
	      config/*.* pam-config.test/*.*
