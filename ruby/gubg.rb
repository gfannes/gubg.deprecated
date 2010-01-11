Dir.chdir(File.dirname(__FILE__)) do
  Dir["gubg/*.rb"].each do |fileName|
    require(fileName)
  end
end