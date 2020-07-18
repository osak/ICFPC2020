require 'net/http'

def main
  begin
    serverurl = ARGV[0]
    playerkey = ARGV[1]
    puts "ServerUrl: %s; PlayerKey: %s" % [serverurl, playerkey]

    uri = URI(serverurl)
    res = Net::HTTP.post(uri, playerkey)
    if res.code == "200"
      puts "Server response: %s" % res.body
    else
      puts "Unexpected server response:"
      puts "HTTP code: %s" % res.code
      puts "Response body: %s" % res.body
      exit(2)
    end
    uri = URI(serverurl)
    res = Net::HTTP.post(uri, "1101100001110110000100")
    if res.code == "200"
      puts "Server response: %s" % res.body
    else
      puts "Unexpected server response:"
      puts "HTTP code: %s" % res.code
      puts "Response body: %s" % res.body
      exit(2)
    end
  rescue StandardError => e
    puts "Unexpected server response:"
    puts e
    puts e.backtrace
    exit(1)
  end
end

if __FILE__ == $0
  main
end