# -*- ruby -*-

require "rubygems"
require "rake/extensiontask"
require "hoe"

# Hoe.plugin :compiler
# Hoe.plugin :gem_prelude_sucks
# Hoe.plugin :inline
# Hoe.plugin :minitest
# Hoe.plugin :racc
# Hoe.plugin :rcov
# Hoe.plugin :rdoc

Hoe.spec "ruby_lirc" do
  # HEY! If you fill these out in ~/.hoe_template/default/Rakefile.erb then
  # you'll never have to touch them again!
  # (delete this comment too, of course)

  developer("Chris Thomas", "cdt3pm@gmail.com")
	self.extra_dev_deps << ['rake-compiler', '>= 0']
	self.spec_extras = { :extensions => ["ext/ruby_lirc_client/extconf.rb"] }

	Rake::ExtensionTask.new('ruby_lirc_client', spec) do |ext|
    ext.lib_dir = File.join('lib', 'ruby_lirc')
  end
end

