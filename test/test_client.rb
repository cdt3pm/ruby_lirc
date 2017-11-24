require 'test/unit'
require_relative '../lib/client'

class TestClient < Test::Unit::TestCase
	def test_initialize
		client = RubyLirc::Client.new
		client.init("testing", 1)
	end
	
	def test_close
	end

	def test_read_config
	end
end
