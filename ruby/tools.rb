Dir.chdir(File.dirname(__FILE__)) do
  Dir["tools/*.rb"].each do |fileName|
    require(fileName)
  end
end
