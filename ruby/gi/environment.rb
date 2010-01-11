require("gubg/utils")
alias old_tempDir tempDir
require("open-uri")
require 'fileutils'

class Environment
  def download(url, name)
    time("Downloading #{name} from #{url}") do
      open(url, 'User-Agent' => 'GenericInstaller').read.export(name)
    end
  end
  def unzip(fileName)
    time("Unzipping \"#{fileName}\"") do
      system("unzip #{fileName}")
    end
  end
  def chmod(permission)
    # TODO: this is a bit hacky and does not take permission into account yet
    time("Chmodding \"#{Dir.pwd}\"") do
      permissionDenied = []
      Dir.each("./", nil, permissionDenied) do |dir, fn|
        system("chmod 777 #{dir}/#{fn}")
        nil
      end
      permissionDenied.each do |file|
        system("chmod 777 #{file}")
      end
    end
  end
  def execute(command)
    time("Executing command \"#{command}\"") do
      system(command)
    end
  end
  def move(from, to)
    time("Moving \"#{from}\" to \"#{to}\"") do
      FileUtils.move(from, to)
    end
  end
  def create(name, content = :dir)
    case content
    when :dir
      time("Creating directory \"#{name}\"") do
        Dir.mkdir(name)
      end
    else
      time("Creating file \"#{name}\"") do
        File.open(name, "wb") do |fo|
          fo.print(content)
        end
      end
    end
  end

  def tempDir
    path = old_tempDir
    Dir.chdir(path) do
      yield
    end
#    FileUtils.rm_rf(path)
  end
end