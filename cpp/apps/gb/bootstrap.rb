#!/usr/bin/env ruby
require("gubg/utils")

localSources = %w[gb]
gubgSources = %w[OptionParser]
objectDir = "objects"
compiler = "g++ -std=c++0x"

Dir.mkdir(objectDir) if !File.exist?(objectDir)

objectExtension = (%w[Linux Linux64].include?(operatingSystem) ? "o" : "obj")

objectFiles = []
[localSources, gubgSources].zip(["", "../../libs/gubg/"]).each do |sources, dir|
    sources.each do |source|
        objectFiles << "#{objectDir}/#{source}.#{objectExtension}"
        cmd = "#{compiler} -c -o#{objectFiles.last} #{dir}#{source}.cpp -I../../libs"
        puts cmd
        system(cmd)
    end
end

cmd = "#{compiler} -ogb-bootstrap #{objectFiles.join(' ')}"
puts cmd
system(cmd)
