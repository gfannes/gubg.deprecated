module Build
    class Executable
        include Rake::DSL
        @@re_cpp = /\.cpp$/
        @@re_hpp = /\.hpp$/
        @@ext_obj = '.obj'
        def initialize(name)
            @name  = name
            @filenames_per_type = Hash.new{|h,k|h[k] = []}
        end
        def add_sources(sources)
            expand_(sources).each do |fn|
                case fn
                when @@re_cpp then @filenames_per_type[:cpp] << fn
                when @@re_hpp then @filenames_per_type[:hpp] << fn
                else puts("WARNING: unknown filetype for #{fn}")
                end
            end
        end
        def create_rules
            reutrn if @rules_are_created
            object_infos = @filenames_per_type[:cpp].map{|fn|{source: fn, object: fn.gsub(@@re_cpp, @@ext_obj)}}
            object_filenames = object_infos.map{|info|info[:object]}
            namespace @name do
                object_infos.each do |info|
                    source = info[:source]
                    object = info[:object]
                    file object do
                        puts("compiling #{source} into #{object}")
                        sh "g++ -std=c++11 -c #{source} -o #{object}"
                    end
                end
                file @name => object_filenames do
                    puts("linker")
                    sh "g++ -o #{@name} #{object_filenames*' '}"
                end
            end
            @rules_are_created = true
        end
        def execute
            create_rules()
            Rake::Task[@name].invoke()
        end

        def expand_(filenames)
            filenames = [filenames].flatten
            filenames.map! do |el|
                case el
                when FileList then el.to_a
                else el end
            end.flatten
        end
    end
end
