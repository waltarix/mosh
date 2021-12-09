AM_CXXFLAGS = -I$(top_srcdir)/ $(WARNING_CXXFLAGS) $(PICKY_CXXFLAGS) $(HARDEN_CFLAGS) $(MISC_CXXFLAGS) $(CODE_COVERAGE_CXXFLAGS)

BUILT_SOURCES = wcwidth9.h

noinst_LIBRARIES = libmoshutil.a

libmoshutil_a_SOURCES = locale_utils.cc locale_utils.h swrite.cc swrite.h dos_assert.h fatal_assert.h select.h select.cc timestamp.h timestamp.cc pty_compat.cc pty_compat.h

wcwidth9.h:
	curl -sL \
		"https://github.com/waltarix/localedata/releases/download/15.1.0-r1/wcwidth9.h" \
		> $@

clean-local:
	@rm -rf wcwidth9.h
