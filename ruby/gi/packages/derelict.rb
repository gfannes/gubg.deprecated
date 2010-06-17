class Install_derelict
  @@url = "http://www.dsource.org/projects/derelict/changeset/head/trunk?old_path=%2F&format=zip"
  @@zipFile = "derelict.zip"
  def initialize(environment)
    @env = environment
  end

  def execute
    @env.tempDir do
      @env.download(@@url, @@zipFile)
      @env.unzip(@@zipFile)
      Dir.chdir("trunk") do
        @env.chmod(0777)
        @env.execute(%q@find ./Derelict* -name "derelict" | xargs -i cp -R '{}' ./@)
        Dir.chdir("derelict") do
          @env.create("root.tree", %Q%
@language[:d] = {
    compiler: { always: "-I../" },
    linker: { always: "" }, 
}
                      %)
          @env.create("lib")
          @env.create("include")
          @env.execute("gb")
        end
        @env.move("derelict", ENV["DSRC"])
      end
    end
  end
end
