#ENV['RC_ARCHS'] = '' if RUBY_PLATFORM =~ /darwin/

require 'mkmf'

dir_config('lirc_client', '/usr/local/include', '/usr/local/lib')
raise 'cannot find lirc library' unless have_library('lirc_client')
create_makefile("ruby_lirc_client/ruby_lirc_client")
