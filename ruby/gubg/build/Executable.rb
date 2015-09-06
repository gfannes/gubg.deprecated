require('gubg/build/GCC.rb')

module Build
    class Executable
        include Rake::DSL
        @@re_cpp = /\.cpp$/
        @@re_hpp = /\.hpp$/
        @@re_sep = /[\.\\\/]/
        @@ext_obj = '.obj'
        def initialize(exe_fn, na = {compiler: nil})
            @exe_fn = exe_fn
            @filenames_per_type = Hash.new{|h,k|h[k] = []}
            compiler_type = case na[:compiler]
                            when NilClass, :gcc then GCC
                            else na[:compiler] end
            @compiler = compiler_type.new
        end

        def exe_filename()
            @exe_fn
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
        def add_include_path(path)
            @compiler.add_include_path(path)
        end

        def create_rules
            return if @rules_are_created
            source_fns_().each do |source|
                object = object_fn_(source)
                file object do
                    sh @compiler.compile_command(object, source)
                end
            end
            object_fns = object_fns_()
            file @exe_fn => object_fns do
                sh @compiler.link_command(@exe_fn, object_fns)
            end
            namespace namespace_name_ do
                task :link => @exe_fn
                task :clean do
                    clean
                end
                task :run do
                    run
                end
            end
            @rules_are_created = true
        end

        #Performs the operations immediately
        def clean
            object_fns_().each{|fn|rm_f(fn)}
            rm_f(@exe_fn)
        end
        def build
            create_rules
            Rake::Task[@exe_fn].invoke()
        end
        def run
            sh "./#{@exe_fn}"
        end

        #Rulenames to be used as rake rule prerequistite
        def clean_rule
            create_rules
            namespace_name_(:clean)
        end
        def build_rule
            create_rules
            namespace_name_(:link)
        end
        def run_rule
            create_rules
            namespace_name_(:run)
        end

        private
        def namespace_name_(t = nil)
            name = "gubg_build_executable_#{@exe_fn}"
            name += ":#{t}" if t
            name
        end
        def source_fns_
            @filenames_per_type[:cpp]
        end
        def object_fn_(source)
            source.gsub(@@re_sep, '_')+@@ext_obj
        end
        def object_fns_
            source_fns_.map{|fn|object_fn_(fn)}
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
