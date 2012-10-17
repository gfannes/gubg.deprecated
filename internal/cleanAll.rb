require("gubg/utils")

system("ce")

#tabReplacement = ' '*8

Dir.each('./') do |dir,fn|
  case fn
  when /\.o$/
  when /\.s$/
  when /\.exec$/
  when /\.exe$/
  when /^a\.out$/
  when /^gmon\.out$/
  when /^execTest/
  when /^test$/
  when /^main$/
  when /_tests$/
  when /_stests$/
  when /\.hex$/
  when /\.unit$/
  when /\.map$/
  when /((\.cpp)|(\.hpp))$/
#    fileName = File.expand_path(fn,dir)
#    str=String.load(fileName)
#    str.gsub("\t",tabReplacement).export(fileName)
    fn = nil
  else
    fn=nil
  end
  if fn
    fn=File.expand_path(fn,dir)
    puts("Removing #{fn}")
    `rm "#{fn}"`
  end
  :ok
end
