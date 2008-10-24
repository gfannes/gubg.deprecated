Dir.chdir(File.dirname(__FILE__)) do
  Dir["*.rb"].each do |fileName|
    require(fileName) unless fileName[/all\.rb$/]
  end
end

