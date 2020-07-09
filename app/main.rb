require 'net/http'

def main
  serverurl = ARGV[0]
  playerkey = ARGV[1]
  puts "ServerUrl: %s; PlayerKey: %s" % [serverurl, playerkey]

  uri = URI(serverurl + "?playerKey=#{playerkey}")
  res = Net::HTTP.get_response(uri)
  puts res
end

if __FILE__ == $0
  main
end