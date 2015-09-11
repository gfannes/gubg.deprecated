class IncludeParser
    @@re_local = /#include "(.+)"/
    @@re_system = /#include <(.+)>/
    def initialize
        @includes_per_fn = Hash.new{|h,k|h[k] = []}
    end
    def extract_includes(fn)
        # puts("extracting includes for #{fn}")
        return @includes_per_fn[fn] if @includes_per_fn.has_key?(fn)
        includes = []
        File.open(fn) do |fo|
            fo.each_line do |line|
                if md = @@re_local.match(line)
                    includes << md[1]
                elsif md = @@re_system.match(line)
                    includes << md[1]
                end
            end
        end
        @includes_per_fn[fn] = includes
        includes
    end
end
