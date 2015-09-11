class FilePool
    @@re_sep = /[^\/\\]+[\/\\](.+)/
    def initialize
        @pool = Hash.new{|h,k|h[k] = []}
    end
    def register(fn_orig)
        fn = fn_orig
        # puts("Registering #{fn_orig} at #{fn}")
        @pool[fn] << fn_orig
        while md = @@re_sep.match(fn)
            fn = md[1]
            # puts("Registering #{fn_orig} at #{fn}")
            @pool[fn] << fn_orig
        end
    end
    def find_files(part)
        @pool[part]
    end
end
