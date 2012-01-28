require("gubg/utils")
require("gubg/token")

$dumpSize = 100

class TranslationUnit
    attr_reader(:file)
    def initialize(na = {file: nil, content: nil})
        @file = na[:file]
        @content = (na[:content] || String.loadBinary(@file)).freeze
        parse_
        parsed = TranslationUnit.generate_(@declarations)
        if @content != parsed
            fnOrig, fnParsed = tempFile("orig.cpp"), tempFile("parsed.cpp")
            @content.export(fnOrig)
            parsed.export(fnParsed)
            system("kdiff3 #{fnOrig} #{fnParsed}")
            raise("Parsed result is different from original")
        end
    end
    def export(filename)
        TranslationUnit.generate_(@declarations).export(filename)
    end
    def each(meta)
        @declarations.each do |decl|
            case decl
            when Hash
                if decl.has_key?(:meta) and decl[:meta] == meta
                    yield(decl)
                end
            end
        end
    end

    private
    @@reInclude = /\#include\s+["<]([^"<>]+)[">]/
        @@reUsing = /using namespace [^;]+;/
        @@reDefine = /\#define .+$/
        @@reUndef = /\#undef .+$/
        @@reExtern = /extern .+$/
        @@reSemiCol = /[^;(){}]+;[\n\r]+/
        def parse_
            @declarations = []
            rest = Token.new(@content)
            while !rest.empty?
                puts("\nrest(#{rest.length}): (((#{rest.hexdump($dumpSize)})))")
                if newline = rest.popNewline
                    puts("Found newline")
                    add_(newline)
                elsif inc = rest.popFront(@@reInclude)
                    puts("Found include #{inc}")
                    add_(inc)
                elsif using = rest.popFront(@@reUsing)
                    puts("Found using #{using}")
                    add_(using)
                elsif ppIf = rest.popFront{|token|TranslationUnit.parsePPIf_(token)}
                    puts("Found pp if of length #{ppIf.length}")
                    add_(ppIf)
                elsif ppDefine = rest.popFront(@@reDefine)
                    puts("Found a define #{ppDefine}")
                    add_(ppDefine)
                elsif ppUndef = rest.popFront(@@reUndef)
                    puts("Found an undef #{ppUndef}")
                    add_(ppUndef)
                elsif extern = rest.popFront(@@reExtern)
                    puts("Found an extern #{extern}")
                    add_(extern)
                elsif namespace = rest.popFront{|token|TranslationUnit.parseNamespace_(token)}
                    puts("Found a namespace")
                    add_(namespace)
                elsif method = rest.popFront{|token|TranslationUnit.parseDeclaration_(token)}
                    puts("Found a declaration of length #{method.length}")
                    add_(method)
                elsif comment = rest.popFront{|token|TranslationUnit.parseComment_(token)}
                    puts("Found a comment")
                    add_(comment)
                elsif semicol = rest.popFront(@@reSemiCol)
                    puts("Found something with a semicol")
                    add_(semicol)
                else
                    raise("Could not parse (((#{rest.hexdump($dumpSize)})))")
                end
            end
        end
    def add_(decl)
        @declarations << decl
    end
    def TranslationUnit.generate_(ary)
        ary.map do |decl|
            case decl
            when Array
                TranslationUnit.generate_(decl)
            when Hash
                TranslationUnit.generate_(decl.values)
            when Symbol
                #This is meant for meta-information passing and won't be serialized
                ""
            else
                decl.to_s
            end
        end.join("")
    end
    @@rePPIf = /\#if.+$/
        @@reLine = /.+$/
        @@rePPEndif = /\#endif/
        def TranslationUnit.parsePPIf_(token)
            ppIf = token.popFront(@@rePPIf)
            return nil if !ppIf
            res = [ppIf]
            verbose = false
            loop do
                puts("\ntoken(#{token.length}): #{token.hexdump($dumpSize)}") if verbose
                if newline = token.popNewline
                    res << newline
                elsif ppEndif = token.popFront(@@rePPEndif)
                    res << ppEndif
                    break
                elsif line = token.popFront(@@reLine)
                    res << line
                else
                    raise("Could not parse #{token.hexdump($dumpSize)}")
                end
            end
            puts("Matched ppif #{res.length}") if verbose
            res
        end
    @@reNamespace = /namespace\s+([a-zA-Z_])\s*/
        @@reNamespaceAnon = /namespace\s*/
        def TranslationUnit.parseNamespace_(token)
            ns = nil
            if ns = token.popFront(@@reNamespace)
            elsif ns = token.popFront(@@reNamespaceAnon)
            else
                return nil
            end
            body = TranslationUnit.parseScope_(token, "{", "}")
            {ns: ns, body: body}
        end
    @@reCtor = /([A-Z][A-Za-z_\d]*::)*([A-Z][A-Za-z_\d]*)::\2/
        @@reDtor = /([A-Z][A-Za-z_\d]*::)*([A-Z][A-Za-z_\d]*)::~\2/
        @@reReturnValue = /((static|const|unsigned)\s+)*(([A-Za-z_()]+::)?[A-Za-z_]+[a-zA-Z_\d()]*\s*[\*&]*)\s+/
        @@reMethod = /([A-Z][A-Za-z_\d]*::)+([A-Za-z_\d]+)/
        @@reFunction = /[a-zA-Z][a-zA-Z\d_<]+/
        def TranslationUnit.parseDeclaration_(token)
            brol = TranslationUnit.parseBrol_(token)
            retv, name = nil, nil
            isCtor, isMethod = false, false
            if name = token.popFront(@@reCtor)
                isCtor, isMethod = true, true
            elsif name = token.popFront(@@reDtor)
                isMethod = true
            elsif retv = token.popFront(@@reReturnValue)
                if name = token.popFront(@@reMethod) or name = token.popFront(@@reFunction)
                end
            end
            return nil if !name
            brol1 = TranslationUnit.parseBrol_(token)
            parameters = TranslationUnit.parseScope_(token, "(", ")")
            return nil if parameters.nil?
            puts("A declaration was found: #{name}...")
            initlist = nil
            if isCtor
                initlist = token.popFront{|t|TranslationUnit.parseInitList_(t)}
            end
            comment = TranslationUnit.parseComment_(token)
            body = TranslationUnit.parseScope_(token, "{", "}")
            if !body
                puts("ERROR::I expected to find a body")
                return nil
            end
            {meta: :method, brol: brol, retv: retv, name: name, brol1: brol1, parameters: parameters, initlist: initlist, comment: comment, body: body}
        end
    def TranslationUnit.parseInitList_(token)
        brol = TranslationUnit.parseBrol_(token)
        dd = token.popFront(":")
        return nil if dd.nil?
        inits = token.popUntil("{")
        [brol, dd, inits]
    end
    def TranslationUnit.parseScope_(token, ot, ct)
        verbose = false
        puts("Parsing scope #{ot} #{ct}") if verbose
        brol = TranslationUnit.parseBrol_(token)
        c = token.content
        six, eix = 0, 0
        oix = c.index(ot, eix)
        return nil if oix != 0
        cix = c.index(ct, eix)
        level = 0
        loop do
            raise("Could not find closing ix") if cix.nil?
            if oix.nil?
                level -= 1
                eix = cix+ct.length
                cix = c.index(ct, eix)
            else
                if oix < cix
                    level += 1
                    eix = oix+ot.length
                    oix = c.index(ot, eix)
                else
                    level -= 1
                    eix = cix+ct.length
                    cix = c.index(ct, eix)
                end
            end
            if level == 0
                eix -= ct.length
                break
            end
        end
        res = {}
        res[:brol] = brol
        res[:open] = token.popFront(ot)
        res[:middle] = token.popFront(c[ot.length, eix-ot.length])
        res[:close] = token.popFront(ct)
        puts("Parsing scope ok: #{res[:middle]}") if verbose
        res
    end
    @@reWhitespace = /\s+/
        def TranslationUnit.parseBrol_(token)
            res = []
            loop do
                if nl = token.popNewline
                    res << nl
                elsif ws = token.popFront(@@reWhitespace)
                    res << ws
                else
                    return res.empty? ? nil : res
                end
            end
        end
    @@reNewstyleComment = /\/\/.+$/
        def TranslationUnit.parseComment_(token)
            res = nil
            token.transaction do
                brol = TranslationUnit.parseBrol_(token)
                if oldStyle = parseScope_(token, "/*", "*/")
                    puts("Old-style comment")
                    res = {brol: brol, comment: oldStyle}
                    :ok
                elsif newStyle = token.popFront(@@reNewstyleComment)
                    puts("New-style comment")
                    res = {brol: brol, comment: newStyle}
                    :ok
                else
                    :failure
                end
            end
            res
        end
end
